#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{

    Ref<TypeSpecifier> FunctionCallExpression::Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType)
    {
        IdentifierExpression *identifierExpression = m_FnName->TryInto<IdentifierExpression>();
        if (!identifierExpression)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Function name must be an identifier expression", true);
        }

        FunctionSymbolNode *fnNode = SymbolIterator().Function(identifierExpression->GetIdentifier().Id());
        if (!fnNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find the function", true);
        }

        if (fnNode->IsMethod() && fnNode->GetSymbolData().Access() != SymbolAccess::Public)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Function declared as private");
        }

        if (fnNode->IsGeneric())
        {
            const std::string name = semanticAnalyzer.InstantiateGenerics(*identifierExpression->GetIdentifier(), m_Generics.get(), this);
            fnNode = SymbolIterator().Function(name);
            ASSERT_A(fnNode != nullptr, "Cannot find the related function");
            m_Generics.reset();
            identifierExpression->GetIdentifier().Id(name);
        }

        const std::vector<Ref<TypeSpecifier>> params = fnNode->Params();

        if (params.size() < m_Args.size())
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Too many params");
            m_ValueType = fnNode->ReturnType();
            return m_ValueType;
        }
        if (params.size() > m_Args.size())
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Missing params");
            m_ValueType = fnNode->ReturnType();
            return m_ValueType;
        }
        if (m_Args.size() == 0)
        {
            m_ValueType = fnNode->ReturnType();
            return m_ValueType;
        }

        for (size_t i = 0; i < m_Args.size(); i++)
        {
            Ref<TypeSpecifier> argType = m_Args.at(i)->Analyze(semanticAnalyzer);
            Ref<TypeSpecifier> paramType = params.at(i);

            if (*argType != *paramType)
            {
                if (!semanticAnalyzer.TryImplicitConversion(m_Args[i], argType, paramType))
                {
                    ErrorSystem::AddError(semanticAnalyzer, this, "Parameter expression type does not match");
                }
            }
        }
        m_ValueType = fnNode->ReturnType();
        return m_ValueType;
    }

    void FunctionCallExpression::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_Generics)
        {
            m_Generics->SubstituteGenerics(genericExpander, map);
        }
        m_FnName->SubstituteGenerics(genericExpander, map);
        for (auto &arg : m_Args)
        {
            arg->SubstituteGenerics(genericExpander, map);
        }
    }

    Box<Expression> FunctionCallExpression::Clone()
    {
        return MakeBox<FunctionCallExpression>(*this);
    }
} // namespace Marble
