#include "Ast/Expressions.hpp"

namespace Marble
{
    Ref<TypeSpecifier> BinaryExpression::Analyze()
    {
        Ref<TypeSpecifier> leftType = m_Left->Analyze();
        Ref<TypeSpecifier> rightType = m_Right->Analyze();

        if (*leftType != *rightType)
        {
            throw "Left and Right hand-side must be the same type";
        }

        if (leftType->IsPrimitive())
        {
            throw "Cannot apply binary operation to complex type";
        }

        if (m_Operator == BinaryOperators::Equal || m_Operator == BinaryOperators::NotEqual ||
            m_Operator == BinaryOperators::And || m_Operator == BinaryOperators::Or ||
            m_Operator == BinaryOperators::GreaterThan || m_Operator == BinaryOperators::LessThan ||
            m_Operator == BinaryOperators::GreaterThanOrEqual ||
            m_Operator == BinaryOperators::LessThanOrEqual)
        {
            return std::make_shared<TypeSpecifier>(Types::Bool);
        }

        return leftType;
    }
} // namespace Marble
