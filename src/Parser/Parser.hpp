#pragma once

#include <stack>
#include "Lexer/Lexer.hpp"
#include "Ast/Program.hpp"
#include "Utils/Memory.hpp"

namespace Marble
{
    struct ParserCheckpoint
    {
        Box<Token> Current;
        Box<Token> Next;
        LexerStates LexerState;
    };
    class Parser
    {
    public:
        Parser(Lexer &lexer);
        ~Parser() = default;

        void NextToken();
        const Token &Expect(TokenType tokenType);
        const Token &NextTokenAndExpect(TokenType tokenType);
        void CreateCheckpoint();
        void RollBack();
        void DiscardCheckpoint();

        inline const Token &Next() const { return *m_Next.get(); }
        inline const Token &Current() const { return *m_Current.get(); }
        inline const Marble::File &File() { return m_Lexer.File(); }

        Ref<Program> Parse();

    private:
        Lexer &m_Lexer;

        Box<Token> m_Current;
        Box<Token> m_Next;
        std::stack<ParserCheckpoint> m_CheckPoints;
    };

} // namespace Marble
