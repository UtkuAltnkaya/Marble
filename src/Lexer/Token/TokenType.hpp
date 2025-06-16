#pragma once
#include <ostream>

namespace Marble
{
    enum class TokenType
    {
        Identifier,

        // OPERATORS START
        Assign,
        Plus,
        Minus,
        Multiply,
        Divide,
        Percent,
        Equal,
        NotEqual,
        LessThan,
        GreaterThan,
        LessEqual,
        GreaterEqual,
        BitAnd,
        BitOr,
        BitXor,
        BitLeft,
        BitRight,
        And,
        Or,
        Bang,
        Tilde,
        Increment,
        Decrement,
        // OPERATORS END

        OpenParen,
        CloseParen,
        OpenCurlyBrace,
        CloseCurlyBrace,
        OpenBracket,
        CloseBracket,

        // KEYWORDS START
        Let,
        Fn,
        Break,
        Case,
        CharKeyword,
        Const,
        Continue,
        Default,
        Do,
        Double,
        Else,
        Enum,
        Float,
        For,
        If,
        Int,
        Bool,
        Return,
        Usize,
        Sizeof,
        Static,
        Struct,
        Switch,
        Void,
        While,
        Str,
        Impl,
        Pub,
        True,
        False,
        Defer,
        As,
        Null,
        // KEYWORDS END

        Number,
        String,
        Char,
        Semicolon,
        Colon,
        Comma,
        Dot,
        Arrow,
        Eof,
    };

    const char *TokenTypeToString(TokenType tokenType);
    std::ostream &operator<<(std::ostream &os, TokenType tokenType);
} // namespace Marble
