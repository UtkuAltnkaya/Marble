#include "Parser/Parser.hpp"

#include "Ast/Expressions.hpp"

namespace Marble
{
    IdentifierExpression::IdentifierExpression(Identifier &&identifier, const Span &span)
        : Expression{span, ExpressionType::Identifier}, m_Identifier{std::move(identifier)}
    {
    }

    Box<Expression> IdentifierExpression::Parse(Parser &parser, Precedence precedence)
    {
        const Token &token = parser.Current();
        if (token.TokenType() != TokenType::Identifier)
        {
            return Expression::Parse(parser, Expression::NextPrecedence(precedence));
        }
        return std::make_unique<IdentifierExpression>(Identifier{token.Text(), token.Span()}, token.Span());
    }
} // namespace Marble
