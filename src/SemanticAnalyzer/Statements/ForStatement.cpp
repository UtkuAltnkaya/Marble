#include "Ast/Statements.hpp"
#include "Utils/IDGenerator.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ForStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *parent = table.CurrentScope();
        SymbolNode *forNode = new SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Block}, parent};
        parent->Insert("for-" + IDGenerator::Generate(), forNode);

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
            throw "Condition type must be boolean";
        }

        m_Increment->Analyze(semanticAnalyzer);
        m_Block->Analyze(semanticAnalyzer);
        table.LeaveScope();
        return TypeSpecifierOk;
    }

    void ForStatement::SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_LetStatement)
        {
            m_LetStatement->SubstituteGenerics(map);
        }
        else
        {
            m_AssignmentExpression->SubstituteGenerics(map);
        }

        m_Condition->SubstituteGenerics(map);
        m_Increment->SubstituteGenerics(map);
        m_Block->SubstituteGenerics(map);
    }

    Box<Statement> ForStatement::Clone()
    {
        return MakeBox<ForStatement>(*this);
    }
} // namespace Marble
