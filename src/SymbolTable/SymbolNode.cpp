#include "SymbolTable/SymbolNode.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "ErrorSystem/ErrorSystem.hpp"
#include "Utils/IDGenerator.hpp"
#include "SymbolNode.hpp"

namespace Marble
{
    SymbolNode::SymbolNode(SymbolData symbolData) : m_SymbolData{symbolData}, m_IsGeneric{false}, m_Block{nullptr}
    {
    }

    SymbolNode::~SymbolNode()
    {
    }

    BlockSymbolNode *SymbolNode::Block()
    {
        if (!m_Block)
        {
            m_Block = new BlockSymbolNode{m_SymbolData.Name() + "_block_" + IDGenerator::Generate(), this};
        }
        return m_Block;
    }

    void SymbolNode::Insert(SymbolNode *node)
    {
        ASSERT_A(m_SymbolData.NodeType() == SymbolNodeTypes::Block, "Node must be block type");
        Into<BlockSymbolNode>()->Insert(node);
    }

    StructSymbolNode::StructSymbolNode(const StructDefinition &structDefinition)
        : SymbolNode{SymbolData{structDefinition.GetName(), SymbolData::FromAccessSpecifier(structDefinition.GetAccessSpecifier()),
                                SymbolNodeTypes::Struct, SymbolNodeBaseTypes::Struct}}
    {
        Block();
        for (auto &field : structDefinition.GetFields())
        {
            auto fieldDefinition = field->Into<StructFieldDefinition>();
            auto node = new VariableSymbolNode(
                fieldDefinition->GetIdentifier().Id(),
                SymbolData::FromAccessSpecifier(fieldDefinition->GetAccessSpecifier()),
                fieldDefinition->GetField().GetTypeSpecifier());
            m_Block->Insert(node);
        }
    }

    void StructSymbolNode::InsertMethod(const std::string &methodName, const std::string &functionName)
    {
        if (m_Methods.contains(methodName))
        {
            ErrorSystem::AddError("Function already inserted");
        }
        m_Methods[methodName] = functionName;
    }

    std::optional<std::reference_wrapper<const std::string>> StructSymbolNode::LookFunctionName(const std::string &methodName)
    {
        if (m_Methods.contains(methodName))
        {
            return m_Methods[methodName];
        }
        return std::nullopt;
    }

    EnumSymbolNode::EnumSymbolNode(const EnumDefinition &enumDefinition)
        : SymbolNode{SymbolData{enumDefinition.GetName(), SymbolData::FromAccessSpecifier(enumDefinition.GetAccessSpecifier()),
                                SymbolNodeTypes::Enum, SymbolNodeBaseTypes::Enum}}
    {
        for (auto &field : enumDefinition.GetFields())
        {
            if (m_EnumFields.contains(field->Id()))
            {
                ErrorSystem::AddError("Duplicate enum field");
            }
            m_EnumFields.insert(field->Id());
        }
    }

    void EnumSymbolNode::InsertMethod(const std::string &methodName, const std::string &functionName)
    {
        if (m_Methods.contains(methodName))
        {
            ErrorSystem::AddError("Function already inserted");
        }
        m_Methods[methodName] = functionName;
    }

    std::optional<std::reference_wrapper<const std::string>> EnumSymbolNode::LookFunctionName(const std::string &methodName)
    {
        if (m_Methods.contains(methodName))
        {
            return m_Methods[methodName];
        }
        return std::nullopt;
    }

    FunctionSymbolNode::FunctionSymbolNode(const FunctionDefinition &fnDefinition, bool isMethod)
        : SymbolNode{SymbolData{fnDefinition.GetName(), SymbolData::FromAccessSpecifier(fnDefinition.GetAccessSpecifier()),
                                SymbolNodeTypes::Function, SymbolNodeBaseTypes::Function}},
          m_IsMethod{isMethod}
    {
        m_ReturnType = fnDefinition.GetReturnType()->Clone();
        Block();
        for (auto &param : fnDefinition.GetParams())
        {
            m_Block->Insert(new VariableSymbolNode{*param});
            m_Params.push_back(param->GetTypeSpecifier()->Clone());
        }
    }

    VariableSymbolNode::VariableSymbolNode(const std::string &name, SymbolAccess access, Ref<TypeSpecifier> typeSpecifier)
        : SymbolNode{SymbolData{name, access, SymbolNodeTypes::Variable, SymbolNodeBaseTypes::Variable}}, m_TypeSpecifier{typeSpecifier}
    {
    }

    VariableSymbolNode::VariableSymbolNode(const VariableType &variableType)
        : VariableSymbolNode{variableType.GetIdentifier().Id(), SymbolAccess::Local, variableType.GetTypeSpecifier()}
    {
    }

    VariableSymbolNode::VariableSymbolNode(const LetStatement &letStmt)
        : VariableSymbolNode{letStmt.GetIdentifier().Id(), SymbolAccess::Local, letStmt.GetTypeSpecifier()}
    {
    }

    BlockSymbolNode::BlockSymbolNode(const std::string &name, SymbolNode *parent)
        : SymbolNode{SymbolData{name, SymbolAccess::Public, SymbolNodeTypes::Block, SymbolNodeBaseTypes::Block}}, m_Parent{parent}
    {
    }

    BlockSymbolNode::~BlockSymbolNode()
    {
        for (auto &[key, value] : m_Children)
        {
            delete value;
        }
        m_Children.clear();
    }

    void BlockSymbolNode::Insert(SymbolNode *node)
    {
        auto &symbolData = node->GetSymbolData();
        if (m_Children.contains(symbolData.Name()))
        {
            ErrorSystem::AddError("Duplicate Identifier");
        }
        m_Children[symbolData.Name()] = node;
    }

    SymbolIterator BlockSymbolNode::Iter()
    {
        return SymbolIterator(this);
    }
} // namespace Marble
