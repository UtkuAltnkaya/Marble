#include "SymbolTable/SymbolNode.hpp"
#include "SymbolNode.hpp"
#include "SymbolTable.hpp"

namespace Marble
{
    SymbolNode::SymbolNode(SymbolData symbolData, SymbolNode *parent)
        : m_SymbolData{symbolData}, m_Parent{parent}
    {
    }

    SymbolNode::SymbolNode(const EnumDefinition &enumDefinition, SymbolNode *parent)
        : SymbolNode{SymbolData{SymbolData::FromAccessSpecifier(enumDefinition.GetAccessSpecifier()), SymbolNodeTypes::Enum, SymbolNodeBaseTypes::None}, parent}
    {
        for (auto &enumField : enumDefinition.GetFields())
        {
            Insert(enumField->Id(), new SymbolNode{SymbolData{SymbolAccess::Public, SymbolNodeTypes::EnumField, SymbolNodeBaseTypes::None}, this});
        }
    }

    SymbolNode::SymbolNode(const StructDefinition &structDefinition, SymbolNode *parent)
        : SymbolNode{SymbolData{SymbolData::FromAccessSpecifier(structDefinition.GetAccessSpecifier()), SymbolNodeTypes::Struct, SymbolNodeBaseTypes::None}, parent}
    {
        for (auto &structField : structDefinition.GetFields())
        {
            Insert(structField->GetField().GetIdentifier().Id(), new VariableSymbolNode{*structField.get(), this});
        }
    }

    SymbolNode::~SymbolNode()
    {
        for (auto &&[key, value] : m_Children)
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
        throw "Duplicate identifier";
    }

    FunctionSymbolNode::FunctionSymbolNode(const FunctionDefinition &fnDefinition, SymbolNode *parent)
        : SymbolNode{
              SymbolData{SymbolData::FromAccessSpecifier(fnDefinition.GetAccessSpecifier()),
                         SymbolNodeTypes::Function, SymbolNodeBaseTypes::Function},
              parent}
    {
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
        : SymbolNode{SymbolData{access, SymbolNodeTypes::Variable, SymbolNodeBaseTypes::Variable}, parent}, m_TypeSpecifier{typeSpecifier}
    {
    }

    VariableSymbolNode::VariableSymbolNode(const VariableType &variableType, SymbolNode *parent)
        : SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Variable, SymbolNodeBaseTypes::Variable}, parent}
    {
        m_TypeSpecifier = variableType.GetTypeSpecifier();
    }

    VariableSymbolNode::VariableSymbolNode(const StructFieldDefinition &structField, SymbolNode *parent)
        : SymbolNode{
              SymbolData{SymbolData::FromAccessSpecifier(structField.GetAccessSpecifier()),
                         SymbolNodeTypes::StructField, SymbolNodeBaseTypes::Variable},
              parent}
    {
        m_TypeSpecifier = structField.GetField().GetTypeSpecifier();
    }

    VariableSymbolNode::VariableSymbolNode(const LetStatement &letStmt, SymbolNode *parent)
        : SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Variable, SymbolNodeBaseTypes::Variable}, parent}
    {
        m_TypeSpecifier = letStmt.GetTypeSpecifier();
    }

} // namespace Marble
