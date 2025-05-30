#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <string_view>

#include "Lexer/Token/Token.hpp"
#include "Utils/File.hpp"
#include "Utils/Memory.hpp"

namespace Marble
{
    struct LexerStates
    {
        size_t Cursor;
        size_t CursorStart;
        size_t Line;
        size_t LineStart;
    };
    class Lexer
    {
    public:
        Lexer(const File &file);
        ~Lexer() = default;

        Box<Token> NextToken();
        LexerStates GetLexerState();
        void SetLexerState(const LexerStates &state);

        inline void SetCursorStart() { m_CursorStart = m_Cursor; }
        inline char CurrentChar() const { return m_Cursor < m_File.Content().size() ? m_File.Content()[m_Cursor] : 0; }
        inline void NextChar() { m_Cursor++; }
        inline const Marble::File &File() const { return m_File; }
        inline size_t Cursor() const { return m_Cursor; }
        inline size_t CursorStart() const { return m_CursorStart; }
        inline size_t Line() const { return m_Line; }
        inline size_t LineStart() const { return m_LineStart; }
        inline const std::unordered_map<std::string_view, TokenType> &Keywords() const { return m_Keywords; }

    private:
        void InitKeywords();
        void SkipSpace();

    private:
        const Marble::File &m_File;
        size_t m_Cursor;
        size_t m_CursorStart;
        size_t m_Line;
        size_t m_LineStart;
        std::unordered_map<std::string_view, TokenType> m_Keywords;
    };

} // namespace Marble
