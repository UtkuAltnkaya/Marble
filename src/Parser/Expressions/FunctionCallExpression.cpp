#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"

namespace Marble
{
    FunctionCallExpression::FunctionCallExpression(Box<Expression> fnName, std::vector<Box<Expression>> &&args, const Span &span)
        : Expression{span, ExpressionType::FunctionCall}, m_FnName{std::move(fnName)}, m_Args{std::move(args)}
    {
    }

    Box<Expression> FunctionCallExpression::Parse(Parser &parser, Precedence precedence)
    {
        Box<Expression> left = Expression::Parse(parser, Expression::NextPrecedence(precedence));

        if (parser.Next().TokenType() != TokenType::OpenParen)
        {
            return left;
        }

        const Span &start = left->GetSpan();
        parser.NextToken();
        std::vector<Box<Expression>> args;
        Parenthesis::Parse<Box<Expression>>(args, parser, TokenType::CloseParen, [](Parser &parser)
                                            { return Expression::Parse(parser); });
        const Span &end = parser.Current().Span();
        Span span{start.Start, end.End};
        return MakeBox<FunctionCallExpression>(std::move(left), std::move(args), span);
    }
} // namespace Marble
