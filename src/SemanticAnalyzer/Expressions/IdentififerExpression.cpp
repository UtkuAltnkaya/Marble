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
        return SymbolIterator(SymbolTable::Get().CurrentScope()).Variable(m_Identifier.Id());
    }

    Box<Expression> IdentifierExpression::Clone()
    {
        return MakeBox<IdentifierExpression>(*this);
    }
} // namespace Marble
