#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"

namespace Marble
{
    ArrayInitExpression::ArrayInitExpression(std::vector<Box<Expression>> &&array, size_t size, const Span &span)
        : Expression{span, ExpressionType::ArrayInit}, m_Array{std::move(array)}, m_Size{size}
    {
    }

    Box<Expression> ArrayInitExpression::Parse(Parser &parser, Precedence precedence)
    {
        if (parser.Current().TokenType() != TokenType::OpenBracket)
        {
            return Expression::Parse(parser, Expression::NextPrecedence(precedence));
        }

        Span start = parser.Current().Span();

        std::vector<Box<Expression>> array;
        Parenthesis::Parse<Box<Expression>>(array, parser, TokenType::CloseBracket, [](Parser &parser)
                                            { return Expression::Parse(parser); });
        size_t size = array.size();
        const Span &end = parser.Current().Span();
        return MakeBox<ArrayIndexExpression>(std::move(array), size, Span{start.Start, end.End});
    }
} // namespace Marble
