#include "Ast/Expressions.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    static SymbolNode *GetFunctionNode(SymbolNode *scope, const std::string &name);

    Ref<TypeSpecifier> FunctionCallExpression::Analyze()
    {
        IdentifierExpression *identifierExpression = m_FnName->TryInto<IdentifierExpression>();
        if (!identifierExpression)
        {
            throw "Function name must be an identifier expression";
        }
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *scope = table.CurrentScope();
        SymbolNode *node = GetFunctionNode(scope, identifierExpression->GetIdentifier().Id());

        if (!node)
        {
            node = GetFunctionNode(table.Root(), identifierExpression->GetIdentifier().Id());
            if (!node)
            {
                throw "Cannot find the function";
            }
        }

        FunctionSymbolNode *fnNode = node->Into<FunctionSymbolNode>();

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
            Ref<TypeSpecifier> argType = m_Args.at(i)->Analyze();
            if (*argType != *params.at(i))
            {
                throw "Parameter expression type does not match";
            }
        }
        return fnNode->ReturnType();
    }

    SymbolNode *GetFunctionNode(SymbolNode *scope, const std::string &name)
    {
        return scope->Iter().Function(name).Find();
    }

} // namespace Marble
