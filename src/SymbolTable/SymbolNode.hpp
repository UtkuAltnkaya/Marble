#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <vector>

#include "Ast/Definitions.hpp"
#include "Ast/Statements.hpp"
#include "Ast/TypeSpecifier.hpp"
#include "Ast/VariableType.hpp"
#include "SymbolTable/SymbolData.hpp"
#include "SymbolTable/SymbolIterator.hpp"
#include "Utils/Macros.hpp"
#include "Utils/Derive/Debug.hpp"

namespace Marble
{
    template <typename T>
    concept SymbolAstVariableNodeType = std::is_base_of_v<Marble::Ast, T> && (std::is_same_v<T, VariableType> || std::is_same_v<T, LetStatement>);

    template <typename T>
    concept SymbolAstStructOrEnumNodeType = std::is_base_of_v<Marble::Ast, T> && (std::is_same_v<T, StructDefinition> || std::is_same_v<T, EnumDefinition>);

    class BlockSymbolNode;
    class SymbolNode : public Derive::Debug
    {
    public:
        friend class SymbolIterator;

        SymbolNode(SymbolData symbolData);
        virtual ~SymbolNode();
        BlockSymbolNode *Block();
        void Insert(SymbolNode *node);

        inline const SymbolData &GetSymbolData() const { return m_SymbolData; }
        inline bool IsGeneric() const { return m_IsGeneric; }

        template <typename T>
        T *TryInto()
        {
            static_assert(std::is_base_of<SymbolNode, T>::value, "It must be base of a SymbolNode");
            if (T::StaticType != m_SymbolData.BaseType())
            {
                return nullptr;
            }
            return static_cast<T *>(this);
        }

        template <typename T>
        T *Into()
        {
            static_assert(std::is_base_of<SymbolNode, T>::value, "It must be base of a SymbolNode");
            ASSERT_D(m_SymbolData.BaseType() == T::StaticType, "Invalid cast in SymbolNode::Into");
            return static_cast<T *>(this);
        }

        template <typename T>
        const T *TryInto() const
        {
            static_assert(std::is_base_of<SymbolNode, T>::value, "It must be base of a SymbolNode");
            if (T::StaticType != m_SymbolData.BaseType())
            {
                return nullptr;
            }
            return static_cast<const T *>(this);
        }

        template <typename T>
        const T *Into() const
        {
            static_assert(std::is_base_of<SymbolNode, T>::value, "It must be base of a SymbolNode");
            ASSERT_D(m_SymbolData.BaseType() == T::StaticType, "Invalid cast in SymbolNode::Into");
            return static_cast<const T *>(this);
        }

    protected:
        const Ast *m_AstNode;
        BlockSymbolNode *m_Block;
        SymbolData m_SymbolData;
        bool m_IsGeneric;
    };

    class StructOrEnumSymbolNode : public SymbolNode
    {
    public:
        static constexpr SymbolNodeBaseTypes StaticType = SymbolNodeBaseTypes::StructOrEnum;
        StructOrEnumSymbolNode(const StructDefinition &structDefinition);
        StructOrEnumSymbolNode(const EnumDefinition &enumDefinition);
        ~StructOrEnumSymbolNode() = default;

        inline const std::unordered_map<std::string, std::string> &Methods() const { return m_Methods; }
        void InsertMethod(const std::string &methodName, const std::string &functionName);
        std::optional<std::reference_wrapper<const std::string>> LookFunctionName(const std::string &methodName);

        template <SymbolAstStructOrEnumNodeType T>
        T *Ast()
        {
            Marble::Ast *node = const_cast<Marble::Ast *>(m_AstNode);
            bool structCondition = m_SymbolData.NodeType() == SymbolNodeTypes::Struct && std::is_same_v<T, StructDefinition>;
            bool enumCondition = m_SymbolData.NodeType() == SymbolNodeTypes::Enum && std::is_same_v<T, EnumDefinition>;
            if (structCondition || enumCondition)
            {
                return static_cast<T *>(node);
            }
            return nullptr;
        }

    private:
        std::unordered_map<std::string, std::string> m_Methods;

        DERIVE_DEBUG(StructOrEnumSymbolNode, FIELD(m_Block), FIELD(m_SymbolData), FIELD(m_Methods))
    };

    class FunctionSymbolNode : public SymbolNode
    {
    public:
        static constexpr SymbolNodeBaseTypes StaticType = SymbolNodeBaseTypes::Function;

        FunctionSymbolNode(const FunctionDefinition &fnDefinition);
        ~FunctionSymbolNode() = default;
        FunctionDefinition *Ast();

        inline const Ref<TypeSpecifier> ReturnType() const { return m_ReturnType; }
        inline const std::vector<Ref<TypeSpecifier>> &Params() const { return m_Params; }
        inline void ReturnType(Ref<TypeSpecifier> returnType) { m_ReturnType = returnType; }
        inline bool IsMethod() const { return m_IsMethod; }

    private:
        bool m_IsMethod;
        Ref<TypeSpecifier> m_ReturnType;
        std::vector<Ref<TypeSpecifier>> m_Params;

        DERIVE_DEBUG(FunctionSymbolNode, FIELD(m_Block), FIELD(m_SymbolData), FIELD(m_IsMethod), FIELD(m_ReturnType), FIELD(m_Params))
    };

    class VariableSymbolNode : public SymbolNode
    {
    public:
        static constexpr SymbolNodeBaseTypes StaticType = SymbolNodeBaseTypes::Variable;

        VariableSymbolNode(const std::string &name, SymbolAccess access, Ref<TypeSpecifier> typeSpecifier);
        VariableSymbolNode(const VariableType &variableType);
        VariableSymbolNode(const LetStatement &letStmt);
        ~VariableSymbolNode() = default;

        inline Ref<TypeSpecifier> GetTypeSpecifier() const { return m_TypeSpecifier; }

        template <SymbolAstVariableNodeType T>
        T *Ast()
        {
            if (!m_AstNode)
            {
                return nullptr;
            }
            Marble::Ast *node = const_cast<Marble::Ast *>(m_AstNode);
            bool letStmtCondition = m_AstNode->GetAstType() == AstType::Statement && std::is_same_v<T, LetStatement>;
            bool variableTypeCondition = m_AstNode->GetAstType() == AstType::VariableType && std::is_same_v<T, VariableType>;
            if (letStmtCondition || variableTypeCondition)
            {
                return static_cast<T *>(node);
            }
            return nullptr;
        }

    private:
        Ref<TypeSpecifier> m_TypeSpecifier;

        DERIVE_DEBUG(VariableSymbolNode, FIELD(m_Block), FIELD(m_SymbolData), FIELD(m_TypeSpecifier))
    };

    class BlockSymbolNode : public SymbolNode
    {
    public:
        friend class SymbolIterator;
        static constexpr SymbolNodeBaseTypes StaticType = SymbolNodeBaseTypes::Block;
        BlockSymbolNode(const std::string &name, SymbolNode *parent);
        ~BlockSymbolNode();

        void Insert(SymbolNode *node);
        SymbolIterator Iter();

        inline size_t Size() const { return m_Children.size(); }
        inline SymbolNode *Parent() const { return m_Parent; }

    private:
        SymbolNode *m_Parent;
        std::unordered_map<std::string, SymbolNode *> m_Children;

        DERIVE_DEBUG(BlockSymbolNode, FIELD(m_Block), FIELD(m_Children))
    };

} // namespace Marble
