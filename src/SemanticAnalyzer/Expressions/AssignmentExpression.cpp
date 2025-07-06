#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{

    Ref<TypeSpecifier> AssignmentExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        CheckVariableExpressionTypes(semanticAnalyzer);
        Ref<TypeSpecifier> left = m_Variable->Analyze(semanticAnalyzer);
        if (left->GetType() == Types::ArrayType)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot assign to array");
        }
        Ref<TypeSpecifier> right = m_Value->Analyze(semanticAnalyzer);

        if (*right != *left)
        {
            std::string message =
                "Left and Right handside types are not matched. Trying to assign " + right->ToString() + " to " + left->ToString();
            ErrorSystem::AddError(semanticAnalyzer, this, message);
        }
        return left;
    }

    void AssignmentExpression::CheckVariableExpressionTypes(SemanticAnalyzer &semanticAnalyzer) const
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
            ErrorSystem::AddError(semanticAnalyzer, this, "Expression must be modifiable value");
        default:
            break;
        }
    }

    void AssignmentExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Value->SubstituteGenerics(semanticAnalyzer, map);
        m_Variable->SubstituteGenerics(semanticAnalyzer, map);
    }

    Box<Expression> AssignmentExpression::Clone()
    {
        return MakeBox<AssignmentExpression>(*this);
    }
} // namespace Marble
