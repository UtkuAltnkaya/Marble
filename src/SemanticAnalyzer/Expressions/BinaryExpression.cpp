#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerError.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    Ref<TypeSpecifier> BinaryExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        Ref<TypeSpecifier> leftType = TypeSpecifier::PassConst(m_Left->Analyze(semanticAnalyzer));
        Ref<TypeSpecifier> rightType = TypeSpecifier::PassConst(m_Right->Analyze(semanticAnalyzer));

        if (*leftType != *rightType)
        {
            Ref<TypeSpecifier> resultType = semanticAnalyzer.UnifyArithmeticTypes(leftType, rightType);
            bool r1 = semanticAnalyzer.TryImplicitConversion(m_Left, leftType, resultType);
            bool r2 = semanticAnalyzer.TryImplicitConversion(m_Right, rightType, resultType);

            if (!r1 || !r2)
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Left and Right hand-side types are not compatible: " + leftType->ToString() + " and " + rightType->ToString());
                return leftType;
            }
            leftType = resultType;
        }

        if (!leftType->IsPrimitive())
        {
            if (leftType->GetType() != Types::UserDefine)
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Cannot apply binary operation to complex type");
            }
            // TODO: Decide, allow operator overloading
            UserDefineTypeKinds kind = leftType->UserDefineUnchecked().Kind;
            if (kind != UserDefineTypeKinds::Enum)
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Cannot apply binary operation to complex type");
            }
        }

        m_ValueType = leftType;
        if (m_Operator == BinaryOperators::Equal || m_Operator == BinaryOperators::NotEqual ||
            m_Operator == BinaryOperators::And || m_Operator == BinaryOperators::Or ||
            m_Operator == BinaryOperators::GreaterThan || m_Operator == BinaryOperators::LessThan ||
            m_Operator == BinaryOperators::GreaterThanOrEqual ||
            m_Operator == BinaryOperators::LessThanOrEqual)
        {
            m_ValueType = std::make_shared<TypeSpecifier>(Types::Bool);
        }
        return m_ValueType;
    }

    void BinaryExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Left->SubstituteGenerics(semanticAnalyzer, map);
        m_Right->SubstituteGenerics(semanticAnalyzer, map);
    }

    Box<Expression> BinaryExpression::Clone()
    {
        return MakeBox<BinaryExpression>(*this);
    }
} // namespace Marble
