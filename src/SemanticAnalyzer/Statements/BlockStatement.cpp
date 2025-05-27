#include "Ast/Statements.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "Utils/IDGenerator.hpp"

namespace Marble
{
    Ref<TypeSpecifier> BlockStatement::Analyze()
    {
        SymbolTable &table = SymbolTable::GetInstance();
        for (auto &statement : m_Statements)
        {
            if (statement->StatementType() == StatementType::Block)
            {
                SymbolNode *currentScope = table.CurrentScope();
                SymbolNode *newScope = new SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Block}, currentScope};
                currentScope->Insert("block-" + IDGenerator::Generate(), newScope);
                table.EnterScope(newScope);
                statement->Analyze();
                table.LeaveScope();
            }
            else
            {
                statement->Analyze();
            }
        }

        return TypeSpecifierOk;
    }
} // namespace Marble
