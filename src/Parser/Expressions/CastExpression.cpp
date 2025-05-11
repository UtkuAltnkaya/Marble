#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    CastExpression::CastExpression(Ref<TypeSpecifier> typeSpecifier, Box<Expression> expression, const Span &span)
        : Expression{span, ExpressionType::Cast}, m_TypeSpecifier{typeSpecifier}, m_Expression{std::move(expression)}
    {
    }

    CastExpression::CastExpression(Ref<TypeSpecifier> typeSpecifier, Box<Expression> expression, Span &&span)
        : Expression{std::move(span), ExpressionType::Cast}, m_TypeSpecifier{typeSpecifier}, m_Expression{std::move(expression)}
    {
    }

    Box<Expression> CastExpression::Parse(Parser &parser, Precedence precedence)
    {
        Box<Expression> left = Expression::Parse(parser, Expression::NextPrecedence(precedence));
        if (parser.Next().TokenType() != TokenType::As)
        {
            return left;
        }

        const Span &start = left->GetSpan();
        parser.NextToken();
        parser.NextToken(); // Skip as token

        Ref<TypeSpecifier> typeSpecifier = TypeSpecifier::Parse(parser);

        const Span &end = typeSpecifier->GetSpan();
        Span span{start.Start, end.End};
        return MakeBox<CastExpression>(typeSpecifier, std::move(left), span);
    }

} // namespace Marble
