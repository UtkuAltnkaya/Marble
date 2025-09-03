#include "SymbolTable/SymbolNode.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "ErrorSystem/ErrorSystem.hpp"
#include "Utils/IDGenerator.hpp"

namespace Marble
{
    SymbolNode::SymbolNode(SymbolData symbolData) : m_SymbolData{symbolData}, m_IsGeneric{false}, m_Block{nullptr}, m_AstNode{nullptr}
    {
    }

    SymbolNode::~SymbolNode()
    {
        if (m_Block)
        {
            delete m_Block;
        }
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

    StructOrEnumSymbolNode::StructOrEnumSymbolNode(const StructDefinition &structDefinition)
        : SymbolNode{SymbolData{structDefinition.GetName(), SymbolData::FromAccessSpecifier(structDefinition.GetAccessSpecifier()),
                                SymbolNodeTypes::Struct, SymbolNodeBaseTypes::StructOrEnum}}
    {
        m_AstNode = &structDefinition;
        m_IsGeneric = structDefinition.GetGenerics() != nullptr;
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

    StructOrEnumSymbolNode::StructOrEnumSymbolNode(const EnumDefinition &enumDefinition)
        : SymbolNode{SymbolData{enumDefinition.GetName(), SymbolData::FromAccessSpecifier(enumDefinition.GetAccessSpecifier()),
                                SymbolNodeTypes::Enum, SymbolNodeBaseTypes::StructOrEnum}}
    {
        m_AstNode = &enumDefinition;
        Block();
        for (auto &field : enumDefinition.GetFields())
        {
            VariableSymbolNode *node =
                new VariableSymbolNode(field->Id(), SymbolAccess::Public, MakeRef<TypeSpecifier>(Types::Int, Span{}));
            m_Block->Insert(node);
        }
    }

    void StructOrEnumSymbolNode::InsertMethod(const std::string &methodName, const std::string &functionName)
    {
        if (m_Methods.contains(methodName))
        {
            ErrorSystem::AddError("Function already inserted");
        }
        m_Methods[methodName] = functionName;
    }

    std::optional<std::reference_wrapper<const std::string>> StructOrEnumSymbolNode::LookFunctionName(const std::string &methodName)
    {
        if (m_Methods.contains(methodName))
        {
            return m_Methods[methodName];
        }
        return std::nullopt;
    }

    llvm::StructType *StructOrEnumSymbolNode::LLVMStructType()
    {
        ASSERT_A(m_SymbolData.NodeType() == SymbolNodeTypes::Struct, "Cannot used with enum symbols");
        return m_LLVMStructType;
    }

    void StructOrEnumSymbolNode::LLVMStructType(llvm::StructType *llvmStructType)
    {
        ASSERT_A(m_SymbolData.NodeType() == SymbolNodeTypes::Struct, "Cannot used with enum symbols");
        m_LLVMStructType = llvmStructType;
    }

    FunctionSymbolNode::FunctionSymbolNode(const FunctionDefinition &fnDefinition)
        : SymbolNode{SymbolData{fnDefinition.GetName(), SymbolData::FromAccessSpecifier(fnDefinition.GetAccessSpecifier()),
                                SymbolNodeTypes::Function, SymbolNodeBaseTypes::Function}}
    {
        m_AstNode = &fnDefinition;
        m_IsGeneric = fnDefinition.GetGenerics() != nullptr;
        m_ReturnType = fnDefinition.GetReturnType()->Clone();
        m_IsMethod = fnDefinition.IsMethod();
        Block();
        for (auto &param : fnDefinition.GetParams())
        {
            m_Block->Insert(new VariableSymbolNode{*param});
            m_Params.push_back(param->GetTypeSpecifier()->Clone());
        }
    }

    FunctionDefinition *FunctionSymbolNode::Ast()
    {
        Marble::Ast *node = const_cast<Marble::Ast *>(m_AstNode);
        return static_cast<FunctionDefinition *>(node);
    }

    VariableSymbolNode::VariableSymbolNode(const std::string &name, SymbolAccess access, Ref<TypeSpecifier> typeSpecifier)
        : SymbolNode{SymbolData{name, access, SymbolNodeTypes::Variable, SymbolNodeBaseTypes::Variable}}, m_TypeSpecifier{typeSpecifier}
    {
    }

    VariableSymbolNode::VariableSymbolNode(const VariableType &variableType)
        : VariableSymbolNode{variableType.GetIdentifier().Id(), SymbolAccess::Local, variableType.GetTypeSpecifier()}
    {
        m_AstNode = &variableType;
    }

    VariableSymbolNode::VariableSymbolNode(const LetStatement &letStmt)
        : VariableSymbolNode{letStmt.GetIdentifier().Id(), SymbolAccess::Local, letStmt.GetTypeSpecifier()}
    {
        m_AstNode = &letStmt;
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
            ErrorSystem::AddError("Duplicate Identifier:" + symbolData.Name());
        }
        m_Children[symbolData.Name()] = node;
    }

    SymbolIterator BlockSymbolNode::Iter()
    {
        return SymbolIterator(this);
    }
} // namespace Marble
