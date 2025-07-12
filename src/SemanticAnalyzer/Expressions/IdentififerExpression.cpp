#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    Ref<TypeSpecifier> IdentifierExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolNode *node = FindNode();
        if (!node)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find the variable in this scope");
            return TypeSpecifierVoid;
        }
        VariableSymbolNode *variableNode = node->Into<VariableSymbolNode>();

        m_ValueType = variableNode->GetTypeSpecifier();
        return m_ValueType;
    }

    SymbolNode *IdentifierExpression::FindNode()
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
            node = iter.Reset().EnumField(m_Identifier.Id()).Find();
            if (node)
            {
                break;
            }
            SymbolNode *parent = iter.Reset().Parent().Find();
            if (parent == nullptr)
            {
                return nullptr;
            }
            iter = parent->Iter();
        } while (node == nullptr);
        return node;
    }

    Box<Expression> IdentifierExpression::Clone()
    {
        return MakeBox<IdentifierExpression>(*this);
    }
} // namespace Marble
