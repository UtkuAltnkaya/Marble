#include "Ast/Statements.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    Ref<TypeSpecifier> LetStatement::Analyze()
    {
        if (m_Value)
        {
            Ref<TypeSpecifier> typeSpecifier = m_Value->Analyze();
            if (!typeSpecifier)
            {
                m_TypeSpecifier = typeSpecifier;
            }
            else if (*m_TypeSpecifier != *typeSpecifier)
            {
                throw "Miss matched types";
            }
        }
        SymbolNode *node = SymbolTable::GetInstance().CurrentScope();
        node->Insert(m_Identifier->Id(), new VariableSymbolNode{*this, node});
        return TypeSpecifierOk;
    }
} // namespace Marble
