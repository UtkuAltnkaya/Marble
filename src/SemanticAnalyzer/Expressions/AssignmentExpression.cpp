#include "Ast/Expressions.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{

    Ref<TypeSpecifier> AssignmentExpression::Analyze()
    {
        CheckVariableExpressionTypes();
        Ref<TypeSpecifier> left = m_Variable->Analyze();
        if (left->GetType() == Types::ArrayType)
        {
            throw "Cannot assign to array";
        }
        Ref<TypeSpecifier> right = m_Value->Analyze();

        if (*right != *left)
        {
            throw "Left and Right handside types are not matched";
        }
        return left;
    }

    void AssignmentExpression::CheckVariableExpressionTypes() const
    {
        switch (m_Variable->ExpressionType())
        {
        case ExpressionType::Unary:
        {
            const UnaryExpression *expr = m_Variable->Into<UnaryExpression>();
            if (expr->GetUnaryExpressionType() == UnaryExpressionType::Prefix && expr->Operator() == UnaryOperators::Indirection)
            {
                break;
            }
        }
        case ExpressionType::Primitive:
        case ExpressionType::ObjectInit:
        case ExpressionType::ArrayInit:
        case ExpressionType::Cast:
        case ExpressionType::NameSpace:
        case ExpressionType::Binary:
            throw "Expression must be modifiable value";
        default:
            break;
        }
    }
} // namespace Marble
