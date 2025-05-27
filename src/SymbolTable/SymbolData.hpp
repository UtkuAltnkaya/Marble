#pragma once

#include "Ast/AccessSpecifier.hpp"
namespace Marble
{
    enum class SymbolAccess
    {
        Local,
        Public,
        Private,
    };

    enum class SymbolNodeTypes
    {
        Global,
        Function,
        Struct,
        StructField,
        Enum,
        EnumField,
        Block,
        Variable,
    };

    class SymbolData
    {
    public:
        SymbolData(SymbolAccess access, SymbolNodeTypes nodeType)
            : m_Access{access}, m_NodeType{nodeType}
        {
        }
        ~SymbolData() = default;

        static SymbolAccess FromAccessSpecifier(AccessSpecifier accessSpecifier)
        {
            SymbolAccess access;
            switch (accessSpecifier)
            {
            case AccessSpecifier::Private:
                access = SymbolAccess::Private;
                break;
            case AccessSpecifier::Public:
                access = SymbolAccess::Public;
                break;
            default:
                access = SymbolAccess::Local;
                break;
            }
            return access;
        }

        inline SymbolAccess Access() const { return m_Access; }
        inline SymbolNodeTypes NodeType() const { return m_NodeType; }
        inline void SetAccess(SymbolAccess access) { m_Access = access; }

    private:
        SymbolAccess m_Access;
        SymbolNodeTypes m_NodeType;
    };

} // namespace Marble
