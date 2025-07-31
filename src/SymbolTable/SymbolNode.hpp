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

namespace Marble
{
    class BlockSymbolNode;
    class SymbolNode
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
        BlockSymbolNode *m_Block;
        SymbolData m_SymbolData;
        bool m_IsGeneric;
    };

    class StructSymbolNode : public SymbolNode
    {
    public:
        static constexpr SymbolNodeBaseTypes StaticType = SymbolNodeBaseTypes::Struct;
        StructSymbolNode(const StructDefinition &structDefinition);
        ~StructSymbolNode() = default;

        inline const std::unordered_map<std::string, std::string> &Methods() const { return m_Methods; }
        void InsertMethod(const std::string &methodName, const std::string &functionName);
        std::optional<std::reference_wrapper<const std::string>> LookFunctionName(const std::string &methodName);

    private:
        std::unordered_map<std::string, std::string> m_Methods;
    };

    class EnumSymbolNode : public SymbolNode
    {
    public:
        static constexpr SymbolNodeBaseTypes StaticType = SymbolNodeBaseTypes::Enum;
        EnumSymbolNode(const EnumDefinition &enumDefinition);
        ~EnumSymbolNode() = default;

        inline const std::unordered_map<std::string, std::string> &Methods() const { return m_Methods; }
        void InsertMethod(const std::string &methodName, const std::string &functionName);
        std::optional<std::reference_wrapper<const std::string>> LookFunctionName(const std::string &methodName);

    private:
        std::unordered_set<std::string> m_EnumFields;
        std::unordered_map<std::string, std::string> m_Methods;
    };

    class FunctionSymbolNode : public SymbolNode
    {
    public:
        static constexpr SymbolNodeBaseTypes StaticType = SymbolNodeBaseTypes::Function;

        FunctionSymbolNode(const FunctionDefinition &fnDefinition, bool isMethod);
        ~FunctionSymbolNode() = default;

        inline const Ref<TypeSpecifier> ReturnType() const { return m_ReturnType; }
        inline const std::vector<Ref<TypeSpecifier>> &Params() const { return m_Params; }
        inline void ReturnType(Ref<TypeSpecifier> returnType) { m_ReturnType = returnType; }
        inline bool IsMethod() const { return m_IsMethod; }

    private:
        bool m_IsMethod;
        Ref<TypeSpecifier> m_ReturnType;
        std::vector<Ref<TypeSpecifier>> m_Params;
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

    private:
        Ref<TypeSpecifier> m_TypeSpecifier;
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
    };

} // namespace Marble
