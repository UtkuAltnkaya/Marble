#include "Lexer/Token/Token.hpp"
#include "Lexer/Lexer.hpp"
#include "Token.hpp"

namespace Marble
{
    Position::Position(size_t row, size_t col, size_t cursor) : Row{row}, Col{col}, Cursor{cursor}
    {
    }

    Span::Span(const Position &start, const Position &end) : Start{start}, End{end}
    {
    }

    Token::Token(Marble::TokenType tokenType, const std::string &text, const Marble::Span &span)
        : m_TokenType{tokenType}, m_Text{text}, m_Span{span}
    {
    }

    Token::Token(Lexer &lexer, Marble::TokenType tokenType)
        : Token{tokenType, lexer.File().Content().substr(lexer.CursorStart(), lexer.Cursor() - lexer.CursorStart()),
                Marble::Span(Position(lexer.Line() + 1, lexer.CursorStart() - lexer.LineStart(), lexer.CursorStart()),
                             Position(lexer.Line() + 1, lexer.Cursor() - lexer.LineStart(), lexer.Cursor()))}
    {
    }

    Token::Token(const Token &token) : Token{token.m_TokenType, token.m_Text, token.m_Span}
    {
    }

} // namespace Marble
