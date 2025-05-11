#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    NamespaceExpression::NamespaceExpression(Box<Expression> namespaceExpr, Box<Expression> value, const Span &span)
        : Expression{span, ExpressionType::NameSpace}, m_Namespace{std::move(namespaceExpr)}, m_Value{std::move(value)}
    {
    }

    Box<Expression> NamespaceExpression::Parse(Parser &parser, Precedence precedence)
    {
        Box<Expression> left = Expression::Parse(parser, Expression::NextPrecedence(precedence));
        if (parser.Next().TokenType() != TokenType::Colon)
        {
            return left;
        }
        const Span &start = left->GetSpan();
        parser.NextToken(); // Skip first colon
        parser.NextTokenAndExpect(TokenType::Colon);
        parser.NextToken(); // Skip second colon

        Box<Expression> value = Expression::Parse(parser, Precedence::FunctionCall);
        const Span &end = value->GetSpan();
        Span span{start.Start, end.End};
        return MakeBox<NamespaceExpression>(std::move(left), std::move(value), span);
    }
} // namespace Marble
