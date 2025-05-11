#pragma once

#include "Lexer/Token/Token.hpp"
#include "Utils/Memory.hpp"

namespace Marble
{
    class Parser;

    enum class AstType
    {
        Program,
        Declaration,
        Statement,
        Expression,
        Identifier,
        TypeSpecifier,
    };

    class Ast
    {
    public:
        Ast(const Span &span, AstType type) : m_Span{span}, m_Type{type} {}
        Ast(Span &&span, AstType type) : m_Span{std::move(span)}, m_Type{type} {}
        virtual ~Ast() = default;

        inline const Span &GetSpan() const { return m_Span; }
        inline AstType GetAstType() const { return m_Type; }

    protected:
        Span m_Span;
        AstType m_Type;
    };

} // namespace Marble
