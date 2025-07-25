#include "SymbolTable/SymbolNode.hpp"
#include "SymbolNode.hpp"
#include "SymbolTable.hpp"

namespace Marble
{
    SymbolNode::SymbolNode(SymbolData symbolData, SymbolNode *parent)
        : m_SymbolData{symbolData}, m_Parent{parent}, m_AstPtr{nullptr}
    {
    }

    SymbolNode::SymbolNode(const EnumDefinition &enumDefinition, SymbolNode *parent)
        : SymbolNode{SymbolData{SymbolData::FromAccessSpecifier(enumDefinition.GetAccessSpecifier()),
                                SymbolNodeTypes::Enum, SymbolNodeBaseTypes::None},
                     parent}
    {
        m_AstPtr = &enumDefinition;
        for (auto &enumField : enumDefinition.GetFields())
        {
            Insert(enumField->Id(), new VariableSymbolNode{*enumField.get(), this, MakeRef<TypeSpecifier>(Types::Int), SymbolNodeTypes::EnumField});
        }
    }

    SymbolNode::~SymbolNode()
    {
        for (auto &[key, value] : m_Children)
        {
            delete value;
        }
    }

    SymbolIterator SymbolNode::Iter() const
    {
        return SymbolIterator(this);
    }

    void SymbolNode::Insert(const std::string &name, SymbolNode *node)
    {
        if (m_Children.find(name) == m_Children.end())
        {
            m_Children.insert({name, node});
            return;
        }
        // TODO Pretty print error
        throw "Duplicate identifier";
    }

    StructSymbolNode::StructSymbolNode(const StructDefinition &structDefinition, SymbolNode *parent)
        : SymbolNode{
              SymbolData{
                  SymbolData::FromAccessSpecifier(structDefinition.GetAccessSpecifier()),
                  SymbolNodeTypes::Struct, SymbolNodeBaseTypes::Struct},
              parent}
    {
        m_StructType = nullptr;
        m_AstPtr = &structDefinition;
        m_IsGeneric = structDefinition.GetGenerics() != nullptr;
        for (auto &structField : structDefinition.GetFields())
        {
            Insert(structField->GetField().GetIdentifier().Id(), new VariableSymbolNode{*structField.get(), this});
        }
    }

    FunctionSymbolNode::FunctionSymbolNode(const FunctionDefinition &fnDefinition, SymbolNode *parent)
        : SymbolNode{
              SymbolData{SymbolData::FromAccessSpecifier(fnDefinition.GetAccessSpecifier()),
                         SymbolNodeTypes::Function, SymbolNodeBaseTypes::Function},
              parent}
    {
        m_AstPtr = &fnDefinition;
        m_ReturnType = fnDefinition.GetReturnType();
        const std::vector<Box<VariableType>> &params = fnDefinition.GetParams();
        m_Params.reserve(params.size());
        m_IsGeneric = fnDefinition.GetGenerics() != nullptr;
        for (auto &param : params)
        {
            m_Params.push_back(param->GetTypeSpecifier());
            Insert(param->GetIdentifier().Id(), new VariableSymbolNode{*param, this});
        }
    }

    FunctionSymbolNode::FunctionSymbolNode(const MemberFunctionDefinition &memberFunction, SymbolNode *parent)
        : SymbolNode{SymbolData{
                         SymbolData::FromAccessSpecifier(memberFunction.GetPrototype().GetAccessSpecifier()),
                         SymbolNodeTypes::Function, SymbolNodeBaseTypes::Function},
                     parent}
    {
        m_AstPtr = &memberFunction;
        const MemberFunctionPrototypeDefinition &prototype = memberFunction.GetPrototype();
        m_ReturnType = prototype.GetReturnType();
        const std::vector<Box<VariableType>> &params = prototype.GetParams();
        m_Params.reserve(params.size());
        m_IsGeneric = prototype.GetGenerics() != nullptr;
        if (auto method = prototype.GetMethod(); method)
        {
            Insert(method->GetIdentifier().Id(), new VariableSymbolNode{*method, this});
        }

        for (auto &param : params)
        {
            m_Params.push_back(param->GetTypeSpecifier());
            Insert(param->GetIdentifier().Id(), new VariableSymbolNode{*param, this});
        }
    }

    VariableSymbolNode::VariableSymbolNode(SymbolAccess access, SymbolNode *parent, Ref<TypeSpecifier> typeSpecifier)
        : SymbolNode{SymbolData{access, SymbolNodeTypes::Variable, SymbolNodeBaseTypes::Variable}, parent}, m_TypeSpecifier{typeSpecifier}, m_Alloca{nullptr}
    {
    }

    VariableSymbolNode::VariableSymbolNode(const VariableType &variableType, SymbolNode *parent)
        : VariableSymbolNode{SymbolAccess::Local, parent, variableType.GetTypeSpecifier()}
    {
        m_AstPtr = &variableType;
    }

    VariableSymbolNode::VariableSymbolNode(const StructFieldDefinition &structField, SymbolNode *parent)
        : SymbolNode{
              SymbolData{SymbolData::FromAccessSpecifier(structField.GetAccessSpecifier()),
                         SymbolNodeTypes::StructField, SymbolNodeBaseTypes::Variable},
              parent},
          m_Alloca{nullptr}
    {
        m_AstPtr = &structField;
        m_TypeSpecifier = structField.GetField().GetTypeSpecifier();
    }

    VariableSymbolNode::VariableSymbolNode(const LetStatement &letStmt, SymbolNode *parent)
        : VariableSymbolNode{SymbolAccess::Local, parent, letStmt.GetTypeSpecifier()}
    {
        m_AstPtr = &letStmt;
    }

    VariableSymbolNode::VariableSymbolNode(const Identifier &identifier, SymbolNode *parent, Ref<TypeSpecifier> ts, SymbolNodeTypes nodeType)
        : VariableSymbolNode{SymbolAccess::Public, parent, ts}
    {
        m_AstPtr = &identifier;
        m_TypeSpecifier = ts;
    }

} // namespace Marble
