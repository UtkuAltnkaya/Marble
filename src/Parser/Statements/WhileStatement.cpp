
#include "Ast/Statements.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    WhileStatement::WhileStatement(Box<Expression> condition, Box<Statement> block, const Span &span)
        : Statement{span, StatementType::While}, m_Condition{std::move(condition)}, m_Block{std::move(block)}
    {
    }

    Box<Statement> WhileStatement::Parse(Parser &parser)
    {
        Span start = parser.Current().Span();
        parser.NextTokenAndExpect(TokenType::OpenParen);
        parser.NextToken();
        Box<Expression> condition = Expression::Parse(parser);
        parser.NextTokenAndExpect(TokenType::CloseBracket);
        Box<Statement> block_stmt = BlockStatement::Parse(parser);
        const Span &end = parser.Current().Span();
        return MakeBox<WhileStatement>(std::move(condition), std::move(block_stmt), Span{start.Start, end.Start});
    }

} // namespace Marble
