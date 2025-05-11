#include "Lexer/Lexer.hpp"
#include "Lexer/Token/TokenFinder.hpp"
#include <cctype>

namespace Marble
{
    Lexer::Lexer(const Marble::File &file) : m_File{file}, m_Cursor{0}, m_CursorStart{0}, m_Line{0}, m_LineStart{0}
    {
        InitKeywords();
    }

    Box<Token> Lexer::NextToken()
    {
        SkipSpace();
        char ch = CurrentChar();

        if (ch == '\0')
        {
            return TokenFinder::Eof(*this);
        }

        if (std::isalpha(ch) || ch == '_')
        {
            return TokenFinder::IdOrKeyword(*this);
        }

        if (ch == '"')
        {
            return TokenFinder::String(*this);
        }

        if (ch == '\'')
        {
            return TokenFinder::Char(*this);
        }

        if (std::isdigit(ch))
        {
            return TokenFinder::Number(*this);
        }

        if (Box<Token> token{TokenFinder::Operator(*this)}; token)
        {
            return token;
        }

        if (Box<Token> token{TokenFinder::Deliminator(*this)}; token)
        {
            return token;
        }

        // throw LexicalError(*this, "Unkown Token!");
        throw "Unkown Token!";
    }

    void Lexer::SkipSpace()
    {
        while (std::isspace(CurrentChar()))
        {
            if (CurrentChar() == '\n')
            {
                m_LineStart = m_Cursor;
                m_Line++;
            }
            NextChar();
        }
    }

    void Lexer::InitKeywords()
    {
        m_Keywords.insert({std::string_view("let"), TokenType::Let});
        m_Keywords.insert({std::string_view("fn"), TokenType::Fn});
        m_Keywords.insert({std::string_view("break"), TokenType::Break});
        m_Keywords.insert({std::string_view("case"), TokenType::Case});
        m_Keywords.insert({std::string_view("char"), TokenType::CharKeyword});
        m_Keywords.insert({std::string_view("const"), TokenType::Const});
        m_Keywords.insert({std::string_view("continue"), TokenType::Continue});
        m_Keywords.insert({std::string_view("default"), TokenType::Default});
        m_Keywords.insert({std::string_view("do"), TokenType::Do});
        m_Keywords.insert({std::string_view("double"), TokenType::Double});
        m_Keywords.insert({std::string_view("else"), TokenType::Else});
        m_Keywords.insert({std::string_view("enum"), TokenType::Enum});
        m_Keywords.insert({std::string_view("float"), TokenType::Float});
        m_Keywords.insert({std::string_view("for"), TokenType::For});
        m_Keywords.insert({std::string_view("if"), TokenType::If});
        m_Keywords.insert({std::string_view("int"), TokenType::Int});
        m_Keywords.insert({std::string_view("bool"), TokenType::Bool});
        m_Keywords.insert({std::string_view("return"), TokenType::Return});
        m_Keywords.insert({std::string_view("usize"), TokenType::Usize});
        m_Keywords.insert({std::string_view("sizeof"), TokenType::Sizeof});
        m_Keywords.insert({std::string_view("static"), TokenType::Static});
        m_Keywords.insert({std::string_view("struct"), TokenType::Struct});
        m_Keywords.insert({std::string_view("switch"), TokenType::Switch});
        m_Keywords.insert({std::string_view("void"), TokenType::Void});
        m_Keywords.insert({std::string_view("while"), TokenType::While});
        m_Keywords.insert({std::string_view("str"), TokenType::Str});
        m_Keywords.insert({std::string_view("impl"), TokenType::Impl});
        m_Keywords.insert({std::string_view("pub"), TokenType::Pub});
        m_Keywords.insert({std::string_view("true"), TokenType::True});
        m_Keywords.insert({std::string_view("false"), TokenType::False});
        m_Keywords.insert({std::string_view("defer"), TokenType::Defer});
        m_Keywords.insert({std::string_view("as"), TokenType::As});
    }

} // namespace Marble