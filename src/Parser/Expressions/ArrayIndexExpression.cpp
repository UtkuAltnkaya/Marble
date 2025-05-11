#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    ArrayIndexExpression::ArrayIndexExpression(Box<Expression> array, Box<Expression> index, Box<Expression> secondIndex, const Span &span)
        : Expression{span, ExpressionType::ArrayIndex}, m_Array{std::move(array)}, m_Index{std::move(index)}, m_SecondIndex{std::move(secondIndex)}
    {
    }

    Box<Expression> ArrayIndexExpression::Parse(Parser &parser, Precedence precedence)
    {
        Box<Expression> left = Expression::Parse(parser, Expression::NextPrecedence(precedence));

        if (parser.Next().TokenType() != TokenType::OpenBracket)
        {
            return left;
        }

        const Span &start = left->GetSpan();
        parser.NextToken(); // Skip array name;
        parser.NextToken(); // Skip open bracket;

        Box<Expression> index = Expression::Parse(parser);
        Box<Expression> secondIndex = nullptr;

        if (parser.Next().TokenType() == TokenType::Comma)
        {
            parser.NextToken();
            parser.NextToken();
            secondIndex = Expression::Parse(parser);
        }

        parser.NextTokenAndExpect(TokenType::CloseBracket);

        const Span &end = parser.Current().Span();
        Span span{start.Start, end.End};

        return MakeBox<ArrayIndexExpression>(std::move(left), std::move(index), std::move(secondIndex), span);
    }

} // namespace Marble
