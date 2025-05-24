#pragma once

#include "Lexer/Token/TokenType.hpp"
#include <memory>
#include <string>

namespace Marble
{
    class Lexer;

    struct Position
    {
        size_t Row;
        size_t Col;
        size_t Cursor;

        Position() = default;
        Position(size_t row, size_t col, size_t cursor);
    };

    struct Span
    {
        Position Start;
        Position End;

        Span() = default;
        Span(const Position &start, const Position &end);
    };

    class Token
    {
    public:
        Token(Marble::TokenType tokenType, const std::string &text, const Span &span);
        Token(Lexer &lexer, Marble::TokenType tokenType);
        ~Token() = default;

    public:
        inline Marble::TokenType TokenType() const { return m_TokenType; }
        inline const std::string &Text() const { return m_Text; }
        inline const Marble::Span &Span() const { return m_Span; }

    private:
        Marble::TokenType m_TokenType;
        std::string m_Text;
        Marble::Span m_Span;
    };

} // namespace Marble
