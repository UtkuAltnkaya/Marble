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

    Box<Statement> ForStatement::Clone()
    {
        return MakeBox<ForStatement>(*this);
    }
} // namespace Marble
