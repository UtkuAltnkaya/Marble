#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{
    Ref<TypeSpecifier> CastExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        Ref<TypeSpecifier> ts = m_Expression->Analyze(semanticAnalyzer);
        if (ts->GetType() == Types::ConstantType)
        {
            ErrorSystem::AddWarn(semanticAnalyzer, this, "Const cast is dangerous avoid using it");
        }
        else if (!ts->IsPrimitive())
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot cast to complex type");
        }
        return m_TypeSpecifier;
    }

    void CastExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_TypeSpecifier->SubstituteGenerics(semanticAnalyzer, map);
        m_Expression->SubstituteGenerics(semanticAnalyzer, map);
    }

    Box<Expression> CastExpression::Clone()
    {
        return MakeBox<CastExpression>(*this);
    }
} // namespace Marble
