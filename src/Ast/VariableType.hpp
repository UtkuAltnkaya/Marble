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
        ~VariableType() = default;
        static Box<VariableType> Parse(Parser &parser);

    private:
        Box<Identifier> m_Identifier;
        Ref<TypeSpecifier> m_TypeSpecifier;
    };

} // namespace Marble
