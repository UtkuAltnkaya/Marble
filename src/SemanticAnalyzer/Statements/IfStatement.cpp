#include "Ast/Statements.hpp"
#include "Utils/IDGenerator.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{

    static void InsertAndAnalyzeBlock(SemanticAnalyzer &semanticAnalyzer, const std::string &name, Statement &blockStatement);

    Ref<TypeSpecifier> IfStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        Ref<TypeSpecifier> condition = m_Condition->Analyze(semanticAnalyzer);

        if (condition->GetType() != Types::Bool)
        {
            throw "Condition type must be boolean";
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
            throw "Condition type must be boolean";
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
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *parent = table.CurrentScope();
        SymbolNode *ifNode = new SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Block}, parent};
        parent->Insert(name + IDGenerator::Generate(), ifNode);

        table.EnterScope(ifNode);
        blockStatement.Analyze(semanticAnalyzer);
        table.LeaveScope();
    }

    void IfStatement::SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Condition->SubstituteGenerics(map);
        m_Block->SubstituteGenerics(map);
        for (auto &elseIf : m_ElseIfStatements)
        {
            elseIf->SubstituteGenerics(map);
        }
        if (m_ElseStatement)
        {
            m_ElseStatement->SubstituteGenerics(map);
        }
    }

    void ElseIfStatement::SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Condition->SubstituteGenerics(map);
        m_Block->SubstituteGenerics(map);
    }

    void ElseStatement::SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Block->SubstituteGenerics(map);
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
