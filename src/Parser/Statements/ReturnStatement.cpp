#include "Ast/Statements.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    ReturnStatement::ReturnStatement(Box<Expression> expression, const Span &span)
        : Statement{span, StatementType::Return}, m_Expression{std::move(expression)}
    {
    }

    Box<Statement> ReturnStatement::Parse(Parser &parser)
    {
        Span start = parser.Current().Span();
        if (parser.Next().TokenType() == TokenType::Semicolon)
        {
            parser.NextToken();
            return MakeBox<ReturnStatement>(nullptr, Span{start.Start, parser.Current().Span().Start});
        }

        parser.NextToken();
        Box<Expression> expression = Expression::Parse(parser);
        parser.NextTokenAndExpect(TokenType::Semicolon);
        return MakeBox<ReturnStatement>(std::move(expression), Span{start.Start, parser.Current().Span().Start});
    }

} // namespace Marble
