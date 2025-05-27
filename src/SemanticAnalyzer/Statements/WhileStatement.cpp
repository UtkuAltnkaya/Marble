#include "Ast/Statements.hpp"
#include "Utils/IDGenerator.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    Ref<TypeSpecifier> WhileStatement::Analyze()
    {
        Ref<TypeSpecifier> condition = m_Condition->Analyze();

        // TODO: Decide whether conditions must bool or bool and int
        if (condition->GetType() != Types::Bool /*&& condition->GetType() != Types::Int*/)
        {
            throw "Condition type must be boolean";
        }

        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *parent = table.CurrentScope();
        SymbolNode *whileNode = new SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Block}, parent};
        parent->Insert("while-" + IDGenerator::Generate(), whileNode);

        table.EnterScope(whileNode);
        m_Block->Analyze();
        table.LeaveScope();
        return TypeSpecifierOk;
    }
} // namespace Marble