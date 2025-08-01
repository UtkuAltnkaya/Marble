#include "Ast/Statements.hpp"
#include "Utils/IDGenerator.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{

    static void InsertAndAnalyzeBlock(SemanticAnalyzer &semanticAnalyzer, const std::string &name, Statement &blockStatement);

    Ref<TypeSpecifier> IfStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        Ref<TypeSpecifier> condition = m_Condition->Analyze(semanticAnalyzer);

        if (condition->GetType() != Types::Bool)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Condition type must be boolean");
        }

        InsertAndAnalyzeBlock(semanticAnalyzer, "if-", *m_Block.get());

        for (auto &elseIfStatement : m_ElseIfStatements)
        {
            elseIfStatement->Analyze(semanticAnalyzer);
        }

        if (m_ElseStatement)
        {
            m_ElseStatement->Analyze(semanticAnalyzer);
        }
        return TypeSpecifierOk;
    }

    Ref<TypeSpecifier> ElseIfStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        Ref<TypeSpecifier> condition = m_Condition->Analyze(semanticAnalyzer);

        if (condition->GetType() != Types::Bool)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Condition type must be boolean");
        }

        InsertAndAnalyzeBlock(semanticAnalyzer, "else-if-", *m_Block.get());
        return TypeSpecifierOk;
    }

    Ref<TypeSpecifier> ElseStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        InsertAndAnalyzeBlock(semanticAnalyzer, "else-", *m_Block.get());
        return TypeSpecifierOk;
    }

    void InsertAndAnalyzeBlock(SemanticAnalyzer &semanticAnalyzer, const std::string &name, Statement &blockStatement)
    {
        SymbolTable &table = SymbolTable::Get();
        SymbolNode *parent = table.CurrentScope();
        BlockSymbolNode *ifNode = new BlockSymbolNode(name + "_" + IDGenerator::Generate(), parent);
        parent->Insert(ifNode);
        table.EnterScope(ifNode);
        blockStatement.Analyze(semanticAnalyzer);
        table.LeaveScope();
    }

    void IfStatement::SubstituteGenerics(
        SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Condition->SubstituteGenerics(semanticAnalyzer, map);
        m_Block->SubstituteGenerics(semanticAnalyzer, map);
        for (auto &elseIf : m_ElseIfStatements)
        {
            elseIf->SubstituteGenerics(semanticAnalyzer, map);
        }
        if (m_ElseStatement)
        {
            m_ElseStatement->SubstituteGenerics(semanticAnalyzer, map);
        }
    }

    void ElseIfStatement::SubstituteGenerics(
        SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Condition->SubstituteGenerics(semanticAnalyzer, map);
        m_Block->SubstituteGenerics(semanticAnalyzer, map);
    }

    void ElseStatement::SubstituteGenerics(
        SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Block->SubstituteGenerics(semanticAnalyzer, map);
    }

    Box<Statement> IfStatement::Clone()
    {
        return MakeBox<IfStatement>(*this);
    }

    Box<Statement> ElseIfStatement::Clone()
    {
        return MakeBox<ElseIfStatement>(*this);
    }

    Box<Statement> ElseStatement::Clone()
    {
        return MakeBox<ElseStatement>(*this);
    }

} // namespace Marble
