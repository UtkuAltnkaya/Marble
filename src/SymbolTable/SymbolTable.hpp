#pragma once

#include <stack>
#include "SymbolTable/SymbolNode.hpp"

namespace Marble
{
    class SymbolScopeGuard
    {
    public:
        SymbolScopeGuard(SymbolNode *node);
        ~SymbolScopeGuard();
    };

    class SymbolTable
    {
    public:
        static void Init();
        static void ShutDown();
        static SymbolTable &Get();

        void Insert(SymbolNode *node);

        void Insert(Definition *definition);
        void Insert(const FunctionDefinition &functionDefinition);
        void Insert(const StructDefinition &structDefinition);
        void Insert(const EnumDefinition &enumDefinition);

        void EnterScope(SymbolNode *node);
        void LeaveScope();
        SymbolNode *CurrentScope();

        inline SymbolNode *Root() { return m_Root; }

    private:
        SymbolTable();
        ~SymbolTable();
        SymbolTable(const SymbolTable &) = delete;
        SymbolTable(SymbolTable &&) = delete;
        SymbolTable &operator=(const SymbolTable &) = delete;
        SymbolTable &&operator=(SymbolTable &&) = delete;

    private:
        SymbolNode *m_Root;
        std::stack<SymbolNode *> m_Scope;
    };

} // namespace Marble
