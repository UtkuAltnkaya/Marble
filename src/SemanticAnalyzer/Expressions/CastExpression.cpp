#include "Ast/Expressions.hpp"

namespace Marble
{
    Ref<TypeSpecifier> CastExpression::Analyze()
    {
        Ref<TypeSpecifier> ts = m_Expression->Analyze();
        if (!ts->IsPrimitive())
        {
            throw "Cannot cast to complex type";
        }
        return m_TypeSpecifier;
    }
} // namespace Marble
