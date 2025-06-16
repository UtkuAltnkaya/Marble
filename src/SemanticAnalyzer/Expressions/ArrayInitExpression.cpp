#include "Ast/Expressions.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ArrayInitExpression::Analyze()
    {
        if (m_Array.size() == 0)
        {
            throw "Cannot create empty array";
        }

        Ref<TypeSpecifier> expressionType = m_Array.at(0)->Analyze();

        for (size_t i = 1; i < m_Array.size(); i++)
        {
            Ref<TypeSpecifier> ts = m_Array.at(i)->Analyze();
            if (*expressionType != *ts)
            {
                throw "Array item type must be same";
            }
        }
        return MakeRef<TypeSpecifier>(ArrayType{expressionType, m_Array.size()}, Span{});
    }
} // namespace Marble
