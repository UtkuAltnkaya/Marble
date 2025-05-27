#pragma once

#include <stack>
#include "SymbolTable/SymbolNode.hpp"

namespace Marble
{
    class SymbolTable
    {
    public:
        static void Init();
        static void ShutDown();
        static SymbolTable &GetInstance();

        void Insert(const std::string &name, SymbolNode *node);
        const SymbolNode *CurrentScope();
        void EnterScope(SymbolNode *node);
        void LeaveScope();
        SymbolIterator Iter();

        inline SymbolNode *Root() const { return m_Root; }

    private:
        SymbolTable();
        ~SymbolTable();
        SymbolTable(const SymbolTable &) = delete;
        SymbolTable &operator=(const SymbolTable &) = delete;

    private:
        SymbolNode *m_Root;
        std::stack<SymbolNode *> m_CurrentScope;
    };

} // namespace Marble
