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

        SymbolIterator Iter();
        void Insert(const std::string &name, SymbolNode *node);
        inline const SymbolData &GetSymbolData() const { return m_SymbolData; }

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
