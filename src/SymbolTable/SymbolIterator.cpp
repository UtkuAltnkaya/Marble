#include "SymbolTable/SymbolIterator.hpp"
#include "SymbolTable/SymbolNode.hpp"
#include "ErrorSystem/ErrorSystem.hpp"
#include "Ast/Ast.hpp"

namespace Marble
{
    SymbolIterator::SymbolIterator()
    {
        m_Node = SymbolTable::Get().Root()->Into<BlockSymbolNode>();
    }

    SymbolIterator::SymbolIterator(SymbolNode *node)
    {
        ASSERT_A(node != nullptr, "Symbol node is nullptr");
        m_Node = node->TryInto<BlockSymbolNode>();
        if (!m_Node)
        {
            ErrorSystem::AddError("Node must be a block symbol");
        }
    }

    FunctionSymbolNode *SymbolIterator::Function(const std::string &name)
    {
        SymbolNode *node = Find(m_Node, name, SymbolNodeTypes::Function);
        if (!node)
        {
            return nullptr;
        }
        return node->Into<FunctionSymbolNode>();
    }

    FunctionSymbolNode *SymbolIterator::Function()
    {
        return Function(m_Node);
    }

    StructSymbolNode *SymbolIterator::Struct(const std::string &name)
    {
        SymbolNode *node = Find(m_Node, name, SymbolNodeTypes::Struct);
        if (!node)
        {
            return nullptr;
        }
        return node->Into<StructSymbolNode>();
    }

    VariableSymbolNode *SymbolIterator::Variable(const std::string &name)
    {
        return Variable(m_Node, name);
    }

    EnumSymbolNode *SymbolIterator::Enum(const std::string &name)
    {
        SymbolNode *node = Find(m_Node, name, SymbolNodeTypes::Enum);
        if (!node)
        {
            return nullptr;
        }
        return node->Into<EnumSymbolNode>();
    }

    FunctionSymbolNode *SymbolIterator::Function(BlockSymbolNode *blockNode)
    {
        if (!blockNode)
        {
            return nullptr;
        }
        if (!blockNode->m_Parent)
        {
            return nullptr;
        }
        FunctionSymbolNode *fnNode = blockNode->m_Parent->TryInto<FunctionSymbolNode>();
        if (fnNode)
        {
            return fnNode;
        }
        return Function(blockNode->m_Parent->TryInto<BlockSymbolNode>());
    }

    VariableSymbolNode *SymbolIterator::Variable(BlockSymbolNode *blockNode, const std::string &name)
    {
        if (!blockNode)
        {
            return nullptr;
        }

        SymbolNode *node = Find(blockNode, name, SymbolNodeTypes::Variable);
        if (node)
        {
            return node->Into<VariableSymbolNode>();
        }

        if (!blockNode->m_Parent)
        {
            return nullptr;
        }
        return Variable(blockNode->m_Parent->TryInto<BlockSymbolNode>(), name);
    }

    SymbolNode *SymbolIterator::Find(BlockSymbolNode *node, const std::string &name, SymbolNodeTypes type)
    {
        auto it = node->m_Children.find(name);

        if (it == node->m_Children.end())
        {
            return nullptr;
        }
        auto symbolNode = it->second;

        if (symbolNode->GetSymbolData().NodeType() != type)
        {
            return nullptr;
        }

        return symbolNode;
    }

} //  namespace Marble
