#include "Ast/Expressions.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    Ref<TypeSpecifier> IdentifierExpression::Analyze()
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *node = nullptr;
        SymbolIterator iter = table.CurrentScope()->Iter();

        do
        {
            node = iter.Variable(m_Identifier.Id()).Find();
            if (node)
            {
                break;
            }
            node = iter.Reset().StructField(m_Identifier.Id()).Find();
            if (node)
            {
                break;
            }
            SymbolNode *parent = iter.Reset().Parent().Find();
            if (parent == nullptr)
            {
                throw "Cannot find the variable";
            }
            iter = parent->Iter();
        } while (node == nullptr);
        VariableSymbolNode *variableNode = node->Into<VariableSymbolNode>();
        return variableNode->GetTypeSpecifier();
    }
} // namespace Marble
