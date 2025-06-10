#include "Ast/Statements.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ReturnStatement::Analyze()
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *parent = table.CurrentScope();

        while (parent && parent->GetSymbolData().NodeType() != SymbolNodeTypes::Function)
        {
            parent = parent->Iter().Parent().Ok().Find();
        }

        FunctionSymbolNode *fnNode = parent->Into<FunctionSymbolNode>();

        if (m_Expression)
        {
            Ref<TypeSpecifier> ts = m_Expression->Analyze();
            if (*ts == *fnNode->ReturnType())
            {
                return TypeSpecifierOk;
            }
        }
        else if (fnNode->ReturnType()->GetType() == Types::Void)
        {
            return TypeSpecifierOk;
        }

        throw "Return value and return type of function does not match";
    }
} // namespace Marble
