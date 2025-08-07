#pragma once

#include "Ast/Ast.hpp"
#include "Ast/Identifier.hpp"
#include "Ast/TypeSpecifier.hpp"

namespace Marble
{
    class VariableType : public Ast
    {
    public:
        VariableType(Box<Identifier> identifier, Ref<TypeSpecifier> typeSpecifier, const Span &span);
        VariableType(const VariableType &obj);
        ~VariableType() = default;
        static Box<VariableType> Parse(Parser &parser);

        inline const Identifier &GetIdentifier() const { return *m_Identifier.get(); }
        inline const Ref<TypeSpecifier> GetTypeSpecifier() const { return m_TypeSpecifier; }
        bool operator==(const VariableType &obj) const { return m_TypeSpecifier->operator==(*obj.m_TypeSpecifier) && *m_Identifier == *obj.m_Identifier; }
        inline void SetIdentifier(Box<Identifier> id) { m_Identifier = std::move(id); }
        Box<VariableType> Clone();

    private:
        Box<Identifier> m_Identifier;
        Ref<TypeSpecifier> m_TypeSpecifier;
    };

} // namespace Marble
