#include "Ast/Statements.hpp"
#include "Utils/IDGenerator.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ForStatement::Analyze()
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *parent = table.CurrentScope();
        SymbolNode *forNode = new SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Block}, parent};
        parent->Insert("for-" + IDGenerator::Generate(), forNode);

        table.EnterScope(forNode);
        if (m_LetStatement)
        {
            m_LetStatement->Analyze();
        }
        else
        {
            m_AssignmentExpression->Analyze();
        }

        Ref<TypeSpecifier> condition = m_Condition->Analyze();

        if (condition->GetType() != Types::Bool)
        {
            table.LeaveScope();
            throw "Condition type must be boolean";
        }

        m_Increment->Analyze();
        m_Block->Analyze();
        table.LeaveScope();
        return TypeSpecifierOk;
    }
} // namespace Marble
