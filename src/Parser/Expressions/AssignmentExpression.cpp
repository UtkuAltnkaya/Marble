#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    AssignmentExpression::AssignmentExpression(Box<Expression> variable, Box<Expression> value, const Span &span)
        : Expression{span, ExpressionType::Assignment}, m_Variable{std::move(variable)}, m_Value{std::move(value)}
    {
    }

    AssignmentExpression::AssignmentExpression(const AssignmentExpression &obj) : Expression{obj.m_Span, ExpressionType::Assignment}
    {
        m_Variable = obj.m_Variable->Clone();
        m_Value = obj.m_Value->Clone();
    }

    Box<Expression> AssignmentExpression::Parse(Parser &parser, Precedence precedence)
    {
        Box<Expression> left = Expression::Parse(parser, Expression::NextPrecedence(precedence));
        if (parser.Next().TokenType() != TokenType::Assign)
        {
            return left;
        }
        const Span &start = left->GetSpan();
        parser.NextToken();
        parser.NextToken();
        Box<Expression> right = Expression::Parse(parser);
        const Span &end = parser.Current().Span();
        Span span{start.Start, end.End};
        return MakeBox<AssignmentExpression>(std::move(left), std::move(right), span);
    }
} // namespace Marble
