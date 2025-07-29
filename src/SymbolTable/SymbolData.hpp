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
        Function,
        Struct,
        Enum,
        Block,
        Variable,
    };

    enum class SymbolNodeBaseTypes
    {
        Variable,
        Function,
        Struct,
        Enum,
        Block,
        None,
    };

    class SymbolData
    {
    public:
        SymbolData(const std::string &name, SymbolAccess access, SymbolNodeTypes nodeType, SymbolNodeBaseTypes baseType = SymbolNodeBaseTypes::None)
            : m_Name{name}, m_Access{access}, m_NodeType{nodeType}, m_BaseType{baseType}
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
        inline SymbolNodeBaseTypes BaseType() const { return m_BaseType; }
        inline const std::string &Name() const { return m_Name; }

        inline void Name(const std::string &name) { m_Name = name; }
        inline void Access(SymbolAccess access) { m_Access = access; }

    private:
        std::string m_Name;
        SymbolAccess m_Access;
        SymbolNodeTypes m_NodeType;
        SymbolNodeBaseTypes m_BaseType;
    };

} // namespace Marble
