#include "Ast/Expressions.hpp"
#include "ErrorSystem/ErrorSystem.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ArrayInitExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        if (m_Array.size() == 0)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot create empty array", true);
        }

        Ref<TypeSpecifier> expressionType = m_Array.at(0)->Analyze(semanticAnalyzer);

        for (size_t i = 1; i < m_Array.size(); i++)
        {
            Ref<TypeSpecifier> ts = m_Array.at(i)->Analyze(semanticAnalyzer);
            if (*expressionType != *ts)
            {
                if (!semanticAnalyzer.TryImplicitConversion(m_Array.at(i), ts, expressionType))
                {
                    ErrorSystem::AddError(semanticAnalyzer, this, "Array item type must be same");
                }
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
