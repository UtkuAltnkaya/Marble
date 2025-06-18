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

    Box<Expression> CastExpression::Clone()
    {
        return MakeBox<CastExpression>(*this);
    }
} // namespace Marble
