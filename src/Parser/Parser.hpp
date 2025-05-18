#pragma once

#include "Lexer/Lexer.hpp"
#include "Ast/Program.hpp"
#include "Utils/Memory.hpp"

namespace Marble
{
    class Parser
    {
    public:
        Parser(Lexer &lexer);
        ~Parser() = default;

        void NextToken();
        const Token &Expect(TokenType tokenType);
        const Token &NextTokenAndExpect(TokenType tokenType);

        inline const Token &Next() const { return *m_Next.get(); }
        inline const Token &Current() const { return *m_Current.get(); }
        inline const Token &Previous() const { return *m_Previous.get(); }
        inline const File &File() { return m_Lexer.File(); }

        Ref<Program> Parse();

    private:
        Lexer &m_Lexer;

        Box<Token> m_Previous;
        Box<Token> m_Current;
        Box<Token> m_Next;
    };

} // namespace Marble
