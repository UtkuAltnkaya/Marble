#include <cctype>
#include "Lexer/Token/TokenFinder.hpp"
#include "TokenFinder.hpp"

namespace Marble
{
    namespace TokenFinder
    {
        static void IsEscapeChar(Lexer &lexer);
        static bool CheckEscape(char ch);
        static bool CheckNextOperator(Lexer &lexer, char ch);

        Box<Token> Eof(Lexer &lexer)
        {
            return MakeBox<Token>(TokenType::Eof,
                                  "EOF",
                                  Span(
                                      Position(lexer.Line() + 1, lexer.Cursor() - lexer.LineStart() + 1, lexer.CursorStart()),
                                      Position(lexer.Line() + 1, lexer.Cursor() - lexer.LineStart() + 1, lexer.Cursor())));
        }

        Box<Token> IdOrKeyword(Lexer &lexer)
        {
            lexer.SetCursorStart();

            while (std::isalpha(lexer.CurrentChar()) || lexer.CurrentChar() == '_' || std::isdigit(lexer.CurrentChar()))
            {
                lexer.NextChar();
            }

            std::string text(lexer.File().Content().substr(lexer.CursorStart(), lexer.Cursor() - lexer.CursorStart()));
            Position start{lexer.Line(), lexer.CursorStart() - lexer.LineStart(), lexer.CursorStart()};
            Position end{lexer.Line(), lexer.Cursor() - lexer.LineStart(), lexer.Cursor()};

            auto keywords = lexer.Keywords();
            TokenType tokenType = TokenType::Identifier;

            if (auto it = keywords.find(text); it != keywords.end())
            {
                tokenType = it->second;
            }
            return MakeBox<Token>(tokenType, text, Span{start, end});
        }

        Box<Token> Number(Lexer &lexer)
        {
            int count(0);
            lexer.SetCursorStart();

            while (std::isdigit(lexer.CurrentChar()) || lexer.CurrentChar() == '.')
            {
                if (lexer.CurrentChar() == '.')
                {
                    count++;
                }
                lexer.NextChar();
            }

            if (count > 1)
            {
                throw "'.' occurs more than once!";
                // throw LexicalError(lexer, "'.' occurs more than once!");
            }

            return MakeBox<Token>(lexer, TokenType::Number);
        }

        Box<Token> String(Lexer &lexer)
        {
            lexer.NextChar();
            lexer.SetCursorStart();

            while (lexer.CurrentChar() != '"')
            {
                if (lexer.Cursor() >= lexer.File().Content().length())
                {
                    throw "\" not closed";
                    // throw LexicalError(lexer, "\" not closed");
                }
                IsEscapeChar(lexer);
                if (lexer.CurrentChar() == '\n')
                {
                    throw "String should not contain new line";
                    // throw LexicalError(lexer, "String should not contain new line");
                }
                lexer.NextChar();
            }

            Box<Token> token = MakeBox<Token>(lexer, TokenType::String);
            lexer.NextChar();
            return token;
        }

        Box<Token> Char(Lexer &lexer)
        {
            lexer.NextChar();
            lexer.SetCursorStart();
            IsEscapeChar(lexer);
            lexer.NextChar();
            Box<Token> token = MakeBox<Token>(lexer, TokenType::Char);
            lexer.NextChar();
            return Box<Token>();
        }

        Box<Token> Deliminator(Lexer &lexer)
        {
            TokenType tokenType;
            switch (lexer.CurrentChar())
            {
            case '(':
                tokenType = TokenType::OpenParen;
                break;
            case ')':
                tokenType = TokenType::CloseParen;
                break;
            case '{':
                tokenType = TokenType::OpenCurlyBrace;
                break;
            case '}':
                tokenType = TokenType::CloseCurlyBrace;
                break;
            case '[':
                tokenType = TokenType::OpenBracket;
                break;
            case ']':
                tokenType = TokenType::CloseBracket;
                break;
            case ',':
                tokenType = TokenType::Comma;
                break;
            case '.':
                tokenType = TokenType::Dot;
                break;
            case ';':
                tokenType = TokenType::Semicolon;
                break;
            case ':':
                tokenType = TokenType::Colon;
                break;
            default:
                return nullptr;
            }
            lexer.NextChar();
            return MakeBox<Token>(lexer, tokenType);
        }

        Box<Token> Operator(Lexer &lexer)
        {
            lexer.SetCursorStart();
            TokenType tokenType;

            switch (lexer.CurrentChar())
            {
            case '+':
                tokenType = CheckNextOperator(lexer, '+') ? TokenType::Increment : TokenType::Plus;
                break;
            case '-':
            {
                if (CheckNextOperator(lexer, '>'))
                {
                    tokenType = TokenType::Arrow;
                }
                else if (CheckNextOperator(lexer, '-'))
                {
                    tokenType = TokenType::Decrement;
                }
                else
                {
                    tokenType = TokenType::Minus;
                }
                break;
            }
            case '*':
                tokenType = TokenType::Multiply;
                break;
            case '/':
                tokenType = TokenType::Divide;
                break;
            case '%':
                tokenType = TokenType::Percent;
                break;
            case '=':
                tokenType = CheckNextOperator(lexer, '=') ? TokenType::Equal : TokenType::Assign;
                break;
            case '!':
                tokenType = CheckNextOperator(lexer, '=') ? TokenType::NotEqual : TokenType::Bang;
                break;
            case '<':
            {
                if (CheckNextOperator(lexer, '<'))
                {
                    tokenType = TokenType::BitLeft;
                }
                else if (CheckNextOperator(lexer, '='))
                {
                    tokenType = TokenType::LessEqual;
                }
                else
                {
                    tokenType = TokenType::LessThan;
                }
                break;
            }
            case '>':
            {
                if (CheckNextOperator(lexer, '>'))
                {
                    tokenType = TokenType::BitRight;
                }
                else if (CheckNextOperator(lexer, '='))
                {
                    tokenType = TokenType::GreaterEqual;
                }
                else
                {
                    tokenType = TokenType::GreaterThan;
                }
                break;
            }
            case '&':
                tokenType = CheckNextOperator(lexer, '&') ? TokenType::And : TokenType::BitAnd;
                break;
            case '|':
                tokenType = CheckNextOperator(lexer, '|') ? TokenType::Or : TokenType::BitOr;
                break;
            case '^':
                tokenType = TokenType::BitOr;
                break;
            case '~':
                tokenType = TokenType::Tilde;
                break;
            default:
                return nullptr;
            }
            lexer.NextChar();
            return MakeBox<Token>(lexer, tokenType);
        }

        void IsEscapeChar(Lexer &lexer)
        {
            if (lexer.CurrentChar() != '\\')
            {
                return;
            }

            lexer.NextChar();
            if (!CheckEscape(lexer.CurrentChar()))
            {
                throw "Unknown escape sequence";
                // throw LexicalError(lexer, "Unknown escape sequence");
            }
        }

        bool CheckEscape(char ch)
        {
            switch (ch)
            {
            case 'a':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
            case 'v':
            case '\\':
            case '\'':
            case '"':
            case '0':
                return true;
            default:
                return false;
            }
        }

        bool CheckNextOperator(Lexer &lexer, char ch)
        {
            if (lexer.Cursor() > lexer.File().Content().size())
            {
                return false;
            }
            if (lexer.File().Content()[lexer.Cursor() + 1] != ch)
            {
                return false;
            }
            lexer.NextChar();
            return true;
        }

    } // namespace TokenFinder

} // namespace Marble
