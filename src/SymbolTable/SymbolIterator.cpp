#include "SymbolTable/SymbolIterator.hpp"
#include "SymbolTable/SymbolNode.hpp"
#include "Ast/Ast.hpp"
#include "SymbolIterator.hpp"

namespace Marble
{
    SymbolIterator::SymbolIterator(SymbolNode *node)
        : m_Node{node}, m_StartNode{node}, m_NodeType{node->m_SymbolData.NodeType()}, m_Flag{false}
    {
    }

    SymbolIterator::SymbolIterator(const SymbolNode *node)
        : m_Node{const_cast<SymbolNode *>(node)}, m_StartNode{const_cast<SymbolNode *>(node)}, m_NodeType{node->m_SymbolData.NodeType()}, m_Flag{false}
    {
    }

    SymbolNode *SymbolIterator::Find()
    {
        if (m_Flag)
        {
            m_Flag = false;
            return nullptr;
        }
        return m_Node;
    }

    SymbolNode *const SymbolIterator::Find(std::string_view name, SymbolNodeTypes nodeType)
    {
        if (m_Flag)
        {
            return m_Node;
        }
        m_NodeType = nodeType;
        auto iter = m_Node->m_Children.find(name.data());
        if (iter == m_Node->m_Children.end())
        {
            m_Flag = true;
            return m_Node;
        }
        SymbolNode *node = iter->second;
        if (node->m_SymbolData.NodeType() != nodeType)
        {
            m_Flag = true;
            return node;
        }
        return node;
    }

    SymbolIterator &SymbolIterator::Parent()
    {
        if (!m_Node->m_Parent)
        {
            m_Flag = true;
        }
        else
        {
            SetNode(m_Node->m_Parent);
        }
        return *this;
    }

    SymbolIterator &SymbolIterator::Function(std::string_view name)
    {
        SymbolNode *node = Find(name, SymbolNodeTypes::Function);
        SetNode(node);
        return *this;
    }

    SymbolIterator &SymbolIterator::Struct(std::string_view name)
    {
        SymbolNode *node = Find(name, SymbolNodeTypes::Struct);
        SetNode(node);
        return *this;
    }

    SymbolIterator &SymbolIterator::Enum(std::string_view name)
    {
        SymbolNode *node = Find(name, SymbolNodeTypes::Enum);
        SetNode(node);
        return *this;
    }

    SymbolIterator &SymbolIterator::Variable(std::string_view name)
    {
        SymbolNode *node = Find(name, SymbolNodeTypes::Variable);
        SetNode(node);
        return *this;
    }

    SymbolIterator &SymbolIterator::StructField(std::string_view name)
    {
        SymbolNode *node = Find(name, SymbolNodeTypes::StructField);
        SetNode(node);
        return *this;
    }
    SymbolIterator &SymbolIterator::Reset()
    {
        SetNode(m_StartNode);
        m_NodeType = m_StartNode->GetSymbolData().NodeType();
        m_Flag = false;
        return *this;
    }

    size_t SymbolIterator::Count(SymbolNodeTypes filter)
    {
        size_t i = 0;
        for (auto &[key, value] : m_Node->m_Children)
        {
            if (filter == value->m_SymbolData.NodeType())
            {
                i++;
            }
        }
        return i;
    }

} //  namespace Marble
