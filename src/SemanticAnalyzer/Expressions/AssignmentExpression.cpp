#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{

    Ref<TypeSpecifier> AssignmentExpression::Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType)
    {
        CheckVariableExpressionTypes(semanticAnalyzer);
        Ref<TypeSpecifier> left = m_Variable->Analyze(semanticAnalyzer);
        if (left->GetType() == Types::ArrayType)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot assign to array");
        }
        else if (left->GetType() == Types::ConstantType)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Expression must be a modifiable value");
        }
        Ref<TypeSpecifier> right = m_Value->Analyze(semanticAnalyzer);

        if (*right == *left)
        {
            m_ValueType = left;
            return m_ValueType;
        }

        if (semanticAnalyzer.TryImplicitConversion(m_Value, right, left))
        {
            m_ValueType = left;
            return m_ValueType;
        }

        std::string message =
            "Left and Right handside types are not matched. Trying to assign " + right->ToString() + " to " + left->ToString();
        ErrorSystem::AddError(semanticAnalyzer, this, message);

        m_ValueType = left;
        return m_ValueType;
    }

    void AssignmentExpression::CheckVariableExpressionTypes(SemanticAnalyzer &semanticAnalyzer) const
    {

        if (m_Variable->IsAssignable())
        {
            return;
        }
        ErrorSystem::AddError(semanticAnalyzer, this, "Expression must be modifiable value");
    }

    void AssignmentExpression::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Value->SubstituteGenerics(genericExpander, map);
        m_Variable->SubstituteGenerics(genericExpander, map);
    }

    Box<Expression> AssignmentExpression::Clone()
    {
        return MakeBox<AssignmentExpression>(*this);
    }
} // namespace Marble
