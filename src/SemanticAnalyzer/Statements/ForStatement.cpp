#include "Ast/Statements.hpp"
#include "Utils/IDGenerator.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ForStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::Get();
        SymbolNode *parent = table.CurrentScope();
        BlockSymbolNode *forNode = new BlockSymbolNode("for_" + IDGenerator::Generate(), parent);
        parent->Insert(forNode);
        table.EnterScope(forNode);
        if (m_LetStatement)
        {
            m_LetStatement->Analyze(semanticAnalyzer);
        }
        else
        {
            m_AssignmentExpression->Analyze(semanticAnalyzer);
        }

        Ref<TypeSpecifier> condition = m_Condition->Analyze(semanticAnalyzer);

        if (condition->GetType() != Types::Bool)
        {
            table.LeaveScope();
            ErrorSystem::AddError(semanticAnalyzer, this, "Condition type must be boolean");
        }

        m_Increment->Analyze(semanticAnalyzer);
        m_Block->Analyze(semanticAnalyzer);
        table.LeaveScope();
        return TypeSpecifierOk;
    }

    void ForStatement::SubstituteGenerics(
        GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_LetStatement)
        {
            m_LetStatement->SubstituteGenerics(genericExpander, map);
        }
        else
        {
            m_AssignmentExpression->SubstituteGenerics(genericExpander, map);
        }

        m_Condition->SubstituteGenerics(genericExpander, map);
        m_Increment->SubstituteGenerics(genericExpander, map);
        m_Block->SubstituteGenerics(genericExpander, map);
    }

    Box<Statement> ForStatement::Clone()
    {
        return MakeBox<ForStatement>(*this);
    }
} // namespace Marble
