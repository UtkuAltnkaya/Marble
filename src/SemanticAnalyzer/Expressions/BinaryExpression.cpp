#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> BinaryExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        Ref<TypeSpecifier> leftType = m_Left->Analyze(semanticAnalyzer);
        Ref<TypeSpecifier>
            rightType = m_Right->Analyze(semanticAnalyzer);

        if (*leftType != *rightType)
        {
            throw "Left and Right hand-side must be the same type";
        }

        // TODO: Decide, allow operator overloading
        if (!leftType->IsPrimitive())
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

    Box<Expression> BinaryExpression::Clone()
    {
        return MakeBox<BinaryExpression>(*this);
    }
} // namespace Marble
