#include "Parser.hpp"

namespace Marble
{
    Parser::Parser(Lexer &lexer) : m_Lexer{lexer}
    {
        m_Previous = nullptr;
        m_Current = m_Lexer.NextToken();
        m_Next = m_Lexer.NextToken();
    }

    void Parser::NextToken()
    {
        m_Previous = std::move(m_Current);
        m_Current = std::move(m_Next);
        m_Next = m_Lexer.NextToken();
    }

    const Token &Parser::Expect(TokenType tokenType)
    {
        if (m_Current->TokenType() != tokenType)
        {
            // throw SyntacticError(*this, std::string("Expect ") + token_type_to_str(token_type) + " but found " + token_type_to_str(this->current->get_token_type()));
            throw std::string("Expect ") + TokenTypeToString(tokenType) + " but found " + TokenTypeToString(m_Current->TokenType());
        }

        return *m_Current.get();
    }

    const Token &Parser::NextTokenAndExpect(TokenType tokenType)
    {
        NextToken();
        return Expect(tokenType);
    }

    Ref<Program> Parser::Parse()
    {
        return Program::Parse(*this);
    }
} // namespace Marble
