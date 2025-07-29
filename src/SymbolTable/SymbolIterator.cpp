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

    VariableSymbolNode *SymbolIterator::Variable(BlockSymbolNode *blockNode, const std::string &name)
    {
        SymbolNode *node = Find(blockNode, name, SymbolNodeTypes::Enum);
        if (node)
        {
            return node->Into<VariableSymbolNode>();
        }
        if (!blockNode->m_Parent)
        {
            return nullptr;
        }
        BlockSymbolNode *parent = blockNode->m_Parent->TryInto<BlockSymbolNode>();
        if (!parent)
        {
            return nullptr;
        }
        return Variable(parent, name);
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
