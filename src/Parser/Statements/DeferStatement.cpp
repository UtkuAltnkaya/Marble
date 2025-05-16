#include "Ast/Statements.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    DeferStatement::DeferStatement(Box<Expression> expression, const Span &span)
        : Statement{span, StatementType::Defer}, m_Expression{std::move(expression)}
    {
    }

    Box<Statement> DeferStatement::Parse(Parser &parser)
    {
        Span start = parser.Current().Span();
        parser.NextToken();
        Box<Expression> expression = Expression::Parse(parser);
        parser.NextTokenAndExpect(TokenType::Semicolon);
        return MakeBox<DeferStatement>(std::move(expression), Span{start.Start, parser.Current().Span().Start});
    }

} // namespace Marble
