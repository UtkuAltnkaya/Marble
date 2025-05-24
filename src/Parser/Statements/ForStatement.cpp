#include "Ast/Statements.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    ForStatement::ForStatement(Box<Statement> letStatement, Box<Expression> condition, Box<Expression> increment, Box<Statement> block, const Span &span)
        : Statement{span, StatementType::For}, m_LetStatement{std::move(letStatement)}, m_AssignmentExpression{nullptr},
          m_Condition{std::move(condition)}, m_increment{std::move(increment)}, m_Block{std::move(block)}
    {
    }
    ForStatement::ForStatement(Box<Expression> assignmentExpression, Box<Expression> condition, Box<Expression> increment, Box<Statement> block, const Span &span)
        : Statement{span, StatementType::For}, m_LetStatement{nullptr}, m_AssignmentExpression{std::move(assignmentExpression)},
          m_Condition{std::move(condition)}, m_increment{std::move(increment)}, m_Block{std::move(block)}
    {
    }

    Box<Statement> ForStatement::Parse(Parser &parser)
    {
        Span start = parser.Current().Span();

        parser.NextTokenAndExpect(TokenType::OpenParen);
        parser.NextToken();

        Box<Statement> letStatement = nullptr;
        Box<Expression> assignmentExpression = nullptr;
        if (parser.Current().TokenType() != TokenType::Let)
        {
            assignmentExpression = std::move(Expression::Parse(parser));
            if (assignmentExpression->ExpressionType() != ExpressionType::Assignment)
            {
                throw "Expect assignment expression";
            }
            parser.NextToken();
        }
        else
        {
            letStatement = std::move(LetStatement::Parse(parser));
        }

        parser.NextToken();
        Box<Expression> condition = Expression::Parse(parser);
        parser.NextTokenAndExpect(TokenType::Semicolon);
        parser.NextToken();

        Box<Expression> increment = Expression::Parse(parser);
        parser.NextTokenAndExpect(TokenType::CloseParen);

        Box<Statement> block = BlockStatement::Parse(parser);

        const Span &end = parser.Current().Span();

        if (letStatement)
        {
            return MakeBox<ForStatement>(std::move(letStatement), std::move(condition), std::move(increment), std::move(block), Span{start.Start, end.Start});
        }
        return MakeBox<ForStatement>(std::move(assignmentExpression), std::move(condition), std::move(increment), std::move(block), Span{start.Start, end.Start});
    }
} // namespace Marble
