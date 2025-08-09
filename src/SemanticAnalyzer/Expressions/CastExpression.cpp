#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{
    Ref<TypeSpecifier> CastExpression::Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType)
    {
        Ref<TypeSpecifier> fromType = m_Expression->Analyze(semanticAnalyzer);
        Ref<TypeSpecifier> toType = m_TypeSpecifier;

        if (fromType->GetType() == Types::ConstantType)
        {
            ErrorSystem::AddWarn(semanticAnalyzer, this, "Const cast is dangerous avoid using it");
        }
        else if (!fromType->IsPrimitive())
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot cast to complex type");
        }

        m_Kind = semanticAnalyzer.CanConvert(fromType, toType);
        if (m_Kind == ConversionKind::Invalid)
        {
            ErrorSystem::AddError(semanticAnalyzer, this,
                                  "Invalid cast from '" + m_TypeSpecifier->ToString() + "' to '" + fromType->ToString() + "'");
        }

        m_ValueType = m_TypeSpecifier;
        return m_ValueType;
    }

    void CastExpression::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_TypeSpecifier->SubstituteGenerics(genericExpander, map);
        m_Expression->SubstituteGenerics(genericExpander, map);
    }

    Box<Expression> CastExpression::Clone()
    {
        return MakeBox<CastExpression>(*this);
    }
} // namespace Marble
