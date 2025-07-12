#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{

    Ref<TypeSpecifier> FunctionCallExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        IdentifierExpression *identifierExpression = m_FnName->TryInto<IdentifierExpression>();
        if (!identifierExpression)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Function name must be an identifier expression", true);
        }
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *scope = table.CurrentScope();
        SymbolNode *node = GetFunctionNode(semanticAnalyzer, scope, identifierExpression->GetIdentifier().Id());
        FunctionSymbolNode *fnNode = node->Into<FunctionSymbolNode>();

        if (fnNode->GetSymbolData().Access() != SymbolAccess::Public)
        {
            if (!node->IsParentRoot())
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Function declared as private");
            }
        }

        if (fnNode->IsGeneric())
        {
            const std::string &name = semanticAnalyzer.InstantiateGenerics(identifierExpression->GetIdentifier().Id(), m_Generics.get());
            node = GetFunctionNode(semanticAnalyzer, scope, name);
            fnNode = node->Into<FunctionSymbolNode>();
            m_Generics.reset();
            identifierExpression->GetIdentifier().Id(name);
        }

        switch (scope->GetSymbolData().NodeType())
        {
        case SymbolNodeTypes::Struct:
        case SymbolNodeTypes::Enum:
            table.LeaveScope();
            break;
        default:
            break;
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

    void FunctionCallExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_Generics)
        {
            m_Generics->SubstituteGenerics(semanticAnalyzer, map);
        }
        m_FnName->SubstituteGenerics(semanticAnalyzer, map);
        for (auto &arg : m_Args)
        {
            arg->SubstituteGenerics(semanticAnalyzer, map);
        }
    }

    SymbolNode *FunctionCallExpression::GetFunctionNode(SemanticAnalyzer &semanticAnalyzer, SymbolNode *scope, const std::string &name)
    {
        SymbolNode *node = scope->Iter().Function(name).Find();
        if (!node)
        {
            SymbolTable &table = SymbolTable::GetInstance();
            node = table.Root()->Iter().Function(name).Find();
            if (!node)
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find the function", true);
            }
        }
        return node;
    }

    Box<Expression> FunctionCallExpression::Clone()
    {
        return MakeBox<FunctionCallExpression>(*this);
    }
} // namespace Marble
