#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ArrayInitExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        if (m_Array.size() == 0)
        {
            throw "Cannot create empty array";
        }

        Ref<TypeSpecifier> expressionType = m_Array.at(0)->Analyze(semanticAnalyzer);

        for (size_t i = 1; i < m_Array.size(); i++)
        {
            Ref<TypeSpecifier> ts = m_Array.at(i)->Analyze(semanticAnalyzer);
            if (*expressionType != *ts)
            {
                throw "Array item type must be same";
            }
        }
        return MakeRef<TypeSpecifier>(ArrayType{expressionType, m_Array.size()}, Span{});
    }

    void ArrayInitExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        for (auto &item : m_Array)
        {
            item->SubstituteGenerics(semanticAnalyzer, map);
        }
    }

    Box<Expression> ArrayInitExpression::Clone()
    {
        return MakeBox<ArrayInitExpression>(*this);
    }
} // namespace Marble
