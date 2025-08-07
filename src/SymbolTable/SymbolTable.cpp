#include <iostream>
#include "SymbolTable/SymbolTable.hpp"
#include "Utils/Macros.hpp"
#include "ErrorSystem/ErrorSystem.hpp"
#include "SymbolTable.hpp"

namespace Marble
{
    static SymbolTable *s_Instance = nullptr;

    SymbolTable::SymbolTable()
    {
        m_Root = new BlockSymbolNode{"Global", nullptr};
        EnterScope(m_Root);
    }

    SymbolTable::~SymbolTable()
    {
        delete m_Root;
    }

    void SymbolTable::Init()
    {
        ASSERT_A(!s_Instance, "Singleton instance is already initalized");
        s_Instance = new SymbolTable();
    }

    void SymbolTable::ShutDown()
    {
        ASSERT_A(s_Instance, "Singleton instance is not initalized");
        delete s_Instance;
    }

    SymbolTable &SymbolTable::Get()
    {
        ASSERT_A(s_Instance, "Singleton instance is not initalized");
        return *s_Instance;
    }

    void SymbolTable::EnterScope(SymbolNode *node)
    {
        m_Scope.push(node);
    }

    void SymbolTable::LeaveScope()
    {
        if (m_Scope.size() == 0)
        {
            ErrorSystem::AddError("Symbol stack underflow");
        }
        m_Scope.pop();
    }

    SymbolNode *SymbolTable::CurrentScope()
    {
        ASSERT_A(m_Scope.size() != 0, "Empty stack");
        return m_Scope.top();
    }

    void SymbolTable::Insert(SymbolNode *node)
    {
        BlockSymbolNode *root = m_Root->Into<BlockSymbolNode>();
        root->Insert(node);
    }

    void SymbolTable::Insert(Definition *definition)
    {
        if (auto f = definition->TryInto<FunctionDefinition>(); f)
        {
            return Insert(*f);
        }

        if (auto s = definition->TryInto<StructDefinition>(); s)
        {
            return Insert(*s);
        }

        if (auto e = definition->TryInto<EnumDefinition>())
        {
            return Insert(*e);
        }

        ASSERT_A(false, "Cannot Insert given definition to symbol table");
    }

    void SymbolTable::Insert(const FunctionDefinition &functionDefinition)
    {
        Insert(new FunctionSymbolNode(functionDefinition));
    }

    void SymbolTable::Insert(const StructDefinition &structDefinition)
    {
        Insert(new StructOrEnumSymbolNode(structDefinition));
    }

    void SymbolTable::Insert(const EnumDefinition &enumDefinition)
    {
        Insert(new StructOrEnumSymbolNode(enumDefinition));
    }

    SymbolScopeGuard::SymbolScopeGuard(SymbolNode *node)
    {
        SymbolTable::Get().EnterScope(node);
    }

    SymbolScopeGuard::~SymbolScopeGuard()
    {
        SymbolTable::Get().LeaveScope();
    }
} // namespace Marble
