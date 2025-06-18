#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"

namespace Marble
{
    FunctionCallExpression::FunctionCallExpression(Box<Expression> fnName, Box<Generics> generics, std::vector<Box<Expression>> &&args, const Span &span)
        : Expression{span, ExpressionType::FunctionCall}, m_FnName{std::move(fnName)}, m_Generics{std::move(generics)}, m_Args{std::move(args)}
    {
    }

    FunctionCallExpression::FunctionCallExpression(const FunctionCallExpression &obj) : Expression{obj.m_Span, ExpressionType::FunctionCall}
    {
        m_FnName = obj.m_FnName->Clone();
        m_Generics = MakeBox<Generics>(*obj.m_Generics.get());
        for (auto &arg : obj.m_Args)
        {
            m_Args.push_back(arg->Clone());
        }
    }

    Box<Expression> FunctionCallExpression::Parse(Parser &parser, Precedence precedence)
    {
        Box<Expression> left = Expression::Parse(parser, Expression::NextPrecedence(precedence));

        Box<Generics> generics = Generics::Parse(parser);

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
        return MakeBox<FunctionCallExpression>(std::move(left), std::move(generics), std::move(args), span);
    }
} // namespace Marble
