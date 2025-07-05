#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{

    Ref<TypeSpecifier> FunctionCallExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        IdentifierExpression *identifierExpression = m_FnName->TryInto<IdentifierExpression>();
        if (!identifierExpression)
        {
            throw "Function name must be an identifier expression";
        }
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *scope = table.CurrentScope();
        SymbolNode *node = GetFunctionNode(scope, identifierExpression->GetIdentifier().Id());
        FunctionSymbolNode *fnNode = node->Into<FunctionSymbolNode>();

        if (fnNode->IsGeneric())
        {
            const std::string &name = semanticAnalyzer.InstantiateGenerics(identifierExpression->GetIdentifier().Id(), m_Generics.get());
            node = GetFunctionNode(scope, name);
            fnNode = node->Into<FunctionSymbolNode>();
        }

        const std::vector<Ref<TypeSpecifier>> params = fnNode->Params();

        if (params.size() < m_Args.size())
        {
            throw "Too many params";
        }
        if (params.size() > m_Args.size())
        {
            throw "Missing params";
        }
        for (size_t i = 1; i < m_Args.size(); i++)
        {
            Ref<TypeSpecifier> argType = m_Args.at(i)->Analyze(semanticAnalyzer);
            if (*argType != *params.at(i))
            {
                throw "Parameter expression type does not match";
            }
        }
        return fnNode->ReturnType();
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

    SymbolNode *FunctionCallExpression::GetFunctionNode(SymbolNode *scope, const std::string &name)
    {
        SymbolNode *node = scope->Iter().Function(name).Find();
        if (!node)
        {
            SymbolTable &table = SymbolTable::GetInstance();
            node = table.Root()->Iter().Function(name).Find();
            if (!node)
            {
                throw "Cannot find the function";
            }
        }
        return node;
    }

    Box<Expression> FunctionCallExpression::Clone()
    {
        return MakeBox<FunctionCallExpression>(*this);
    }
} // namespace Marble
