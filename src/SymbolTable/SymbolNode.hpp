#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Ast/Definitions.hpp"
#include "Ast/Statements.hpp"
#include "Ast/TypeSpecifier.hpp"
#include "Ast/VariableType.hpp"
#include "SymbolTable/SymbolData.hpp"
#include "SymbolTable/SymbolIterator.hpp"

namespace Marble
{
    class SymbolNode
    {
    public:
        friend class SymbolIterator;

        SymbolNode(SymbolData symbolData, SymbolNode *parent);
        SymbolNode(const EnumDefinition &enumDefinition, SymbolNode *parent);
        SymbolNode(const StructDefinition &structDefinition, SymbolNode *parent);
        virtual ~SymbolNode();

        SymbolIterator Iter() const;
        void Insert(const std::string &name, SymbolNode *node);
        inline const SymbolData &GetSymbolData() const { return m_SymbolData; }

        template <typename T>
        T *TryInto()
        {
            static_assert(std::is_base_of<SymbolNode, T>::value, "It must be base of a SymbolNode");
            if (T::StaticType != m_ExpressionType)
            {
                throw "Expression types are not matched";
            }
            return static_cast<T *>(this);
        }

        template <typename T>
        T *Into()
        {
            static_assert(std::is_base_of<SymbolNode, T>::value, "It must be base of a SymbolNode");
            assert(m_ExpressionType == T::StaticType && "Invalid cast in SymbolNode::Into");
            return static_cast<T *>(this);
        }

        template <typename T>
        const T *TryInto() const
        {
            static_assert(std::is_base_of<SymbolNode, T>::value, "It must be base of a SymbolNode");
            if (T::StaticType != m_ExpressionType)
            {
                throw "Expression types are not matched";
            }
            return static_cast<const T *>(this);
        }

        template <typename T>
        const T *Into() const
        {
            static_assert(std::is_base_of<SymbolNode, T>::value, "It must be base of a SymbolNode");
            assert(m_ExpressionType == T::StaticType && "Invalid cast in SymbolNode::Into");
            return static_cast<const T *>(this);
        }

    protected:
        SymbolData m_SymbolData;
        SymbolNode *m_Parent;
        std::unordered_map<std::string, SymbolNode *> m_Children;
    };

    class FunctionSymbolNode : public SymbolNode
    {
    public:
        FunctionSymbolNode(const FunctionDefinition &fnDefinition, SymbolNode *parent);
        FunctionSymbolNode(const MemberFunctionDefinition &memberFunction, SymbolNode *parent);
        ~FunctionSymbolNode() = default;

        inline const Ref<TypeSpecifier> ReturnType() const { return m_ReturnType; }
        inline const std::vector<Ref<TypeSpecifier>> &Params() const { return m_Params; }

    private:
        Ref<TypeSpecifier> m_ReturnType;
        std::vector<Ref<TypeSpecifier>> m_Params;
    };

    class VariableSymbolNode : public SymbolNode
    {
    public:
        VariableSymbolNode(SymbolAccess access, SymbolNode *parent, Ref<TypeSpecifier> typeSpecifier);
        VariableSymbolNode(const VariableType &variableType, SymbolNode *parent);
        VariableSymbolNode(const StructFieldDefinition &structField, SymbolNode *parent);
        VariableSymbolNode(const LetStatement &letStmt, SymbolNode *parent);
        ~VariableSymbolNode() = default;

        inline Ref<TypeSpecifier> GetTypeSpecifier() const { return m_TypeSpecifier; }

    private:
        Ref<TypeSpecifier> m_TypeSpecifier;
    };

} // namespace Marble
