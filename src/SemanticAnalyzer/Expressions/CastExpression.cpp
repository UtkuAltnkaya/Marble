#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> CastExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        Ref<TypeSpecifier> ts = m_Expression->Analyze(semanticAnalyzer);
        if (!ts->IsPrimitive())
        {
            throw "Cannot cast to complex type";
        }
        return m_TypeSpecifier;
    }

    void CastExpression::SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_TypeSpecifier->SubstituteGenerics(map);
        m_Expression->SubstituteGenerics(map);
    }

    Box<Expression> CastExpression::Clone()
    {
        return MakeBox<CastExpression>(*this);
    }
} // namespace Marble
