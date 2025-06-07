#include "Ast/Expressions.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{

    static SymbolNode *GetFunctionNode();
    Ref<TypeSpecifier> FunctionCallExpression::Analyze()
    {
        IdentifierExpression *identifierExpression = m_FnName->TryInto<IdentifierExpression>();
        SymbolNode *node = GetFunctionNode();
        FunctionSymbolNode *fnNode = node->Iter()
                                         .Function(identifierExpression->GetIdentifier().Id())
                                         .Ok()
                                         .Find()
                                         ->Into<FunctionSymbolNode>();
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

    SymbolNode *GetFunctionNode()
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *scope = table.CurrentScope();
        SymbolNodeTypes nodeType = scope->GetSymbolData().NodeType();
        if (nodeType != SymbolNodeTypes::Enum && nodeType != SymbolNodeTypes::Struct)
        {
            return table.Root();
        }
        // Leave enum or struct scope
        table.LeaveScope();
        return scope;
    }
} // namespace Marble
