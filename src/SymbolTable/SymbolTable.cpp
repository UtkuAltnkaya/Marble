#include <iostream>
#include "SymbolTable/SymbolTable.hpp"
#include "Utils/Macros.hpp"

namespace Marble
{
    static SymbolTable *s_Instance = nullptr;

    SymbolTable::SymbolTable()
    {
        m_Root = new SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Global, SymbolNodeBaseTypes::None}, nullptr};
        m_CurrentScope.push(m_Root);
    }

    SymbolTable::~SymbolTable()
    {
        if (m_Root)
        {
            delete m_Root;
        }
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

    SymbolTable &SymbolTable::GetInstance()
    {
        ASSERT_A(s_Instance, "Singleton instance is not initalized");
        return *s_Instance;
    }

    void SymbolTable::Insert(const std::string &name, SymbolNode *node)
    {
        m_Root->Insert(name, node);
    }

    SymbolNode *const SymbolTable::CurrentScope()
    {
        return m_CurrentScope.top();
    }

    void SymbolTable::EnterScope(SymbolNode *node)
    {
        m_CurrentScope.push(node);
    }

    void SymbolTable::LeaveScope()
    {
        m_CurrentScope.pop();
    }

    SymbolIterator SymbolTable::Iter()
    {
        return SymbolIterator(m_Root);
    }

} // namespace Marble
