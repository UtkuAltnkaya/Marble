#include <assert.h>
#include "Parser.hpp"
#include "Utils/Macros.hpp"
#include "Lexer/Token/TokenType.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    Parser::Parser(Lexer &lexer) : m_Lexer{lexer}
    {
        m_Current = m_Lexer.NextToken();
        m_Next = m_Lexer.NextToken();
    }

    void Parser::NextToken()
    {
        m_Current = std::move(m_Next);
        m_Next = m_Lexer.NextToken();
    }

    const Token &Parser::Expect(TokenType tokenType)
    {
        if (m_Current->TokenType() != tokenType)
        {
            std::string errMessage = std::string("Expect ") + TokenTypeToString(tokenType) + " but found " + TokenTypeToString(m_Current->TokenType());
            ErrorSystem::AddError(*this, errMessage);
        }

        return *m_Current.get();
    }

    const Token &Parser::NextTokenAndExpect(TokenType tokenType)
    {
        NextToken();
        return Expect(tokenType);
    }

    void Parser::CreateCheckpoint()
    {
        m_CheckPoints.push(ParserCheckpoint{
            MakeBox<Token>(*m_Current),
            MakeBox<Token>(*m_Next),
            m_Lexer.GetLexerState()});
    }

    void Parser::RollBack()
    {
        ASSERT_A(!m_CheckPoints.empty(), "No checkpoint to rollback");

        ParserCheckpoint checkPoint = std::move(m_CheckPoints.top());
        m_CheckPoints.pop();

        m_Current = std::move(checkPoint.Current);
        m_Next = std::move(checkPoint.Next);
        m_Lexer.SetLexerState(checkPoint.LexerState);
    }

    void Parser::DiscardCheckpoint()
    {
        if (!m_CheckPoints.empty())
        {
            m_CheckPoints.pop();
        }
    }

    Ref<Program> Parser::Parse()
    {
        return Program::Parse(*this);
    }
} // namespace Marble
