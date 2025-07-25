#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{

    UnaryExpression::UnaryExpression(UnaryOperators unaryOperator, Box<Expression> value, UnaryExpressionType expressionType, const Span &span)
        : Expression{span, ExpressionType::Unary}, m_UnaryOperator{unaryOperator}, m_Value{std::move(value)}, m_UnaryExpressionType{expressionType}
    {
    }
    UnaryExpression::UnaryExpression(UnaryOperators unaryOperator, Box<Expression> value, UnaryExpressionType expressionType, Span &&span)
        : Expression{std::move(span), ExpressionType::Unary}, m_UnaryOperator{unaryOperator}, m_Value{std::move(value)}, m_UnaryExpressionType{expressionType}
    {
    }

    UnaryExpression::UnaryExpression(const UnaryExpression &obj) : Expression{obj.m_Span, ExpressionType::Unary}
    {
        m_UnaryOperator = obj.m_UnaryOperator;
        m_Value = obj.m_Value->Clone();
        m_UnaryExpressionType = obj.m_UnaryExpressionType;
        if (obj.m_ValueType)
        {
            m_ValueType = MakeRef<TypeSpecifier>(*obj.m_ValueType.get());
        }
    }

    Box<Expression> UnaryExpression::Parse(Parser &parser, Precedence precedence)
    {
        if (auto prefix = UnaryExpression::ParsePrefix(parser, precedence); prefix)
        {
            return prefix;
        }
        Box<Expression> left{Expression::Parse(parser, Expression::NextPrecedence(precedence))};

        UnaryOperators unaryOperator;
        switch (parser.Next().TokenType())
        {
        case TokenType::Increment:
            unaryOperator = UnaryOperators::Increment;
            break;
        case TokenType::Decrement:
            unaryOperator = UnaryOperators::Decrement;
            break;
        default:
            return left;
        }

        Span start = left->GetSpan();
        Span end = parser.Next().Span();

        parser.NextToken();
        return MakeBox<UnaryExpression>(unaryOperator, std::move(left), UnaryExpressionType::PostFix, Span{start.Start, end.End});
    }

    Box<Expression> UnaryExpression::ParsePrefix(Parser &parser, Precedence precedence)
    {
        UnaryOperators unary_operator;
        if (auto op = UnaryExpression::TokenTypeToOperator(parser.Current().TokenType()); op)
        {
            unary_operator = op.value();
        }
        else
        {
            return nullptr;
        }

        Span start = parser.Current().Span();

        parser.NextToken();
        Box<Expression> value{Expression::Parse(parser, Expression::NextPrecedence(precedence))};

        const Span &end = value->GetSpan();
        Span span{start.Start, end.End};
        return MakeBox<UnaryExpression>(unary_operator, std::move(value), UnaryExpressionType::Prefix, span);
    }

    std::optional<UnaryOperators> UnaryExpression::TokenTypeToOperator(TokenType tokenType)
    {
        switch (tokenType)
        {
        case TokenType::Increment:
            return UnaryOperators::Increment;
        case TokenType::Decrement:
            return UnaryOperators::Decrement;
        case TokenType::Plus:
            return UnaryOperators::Plus;
        case TokenType::Minus:
            return UnaryOperators::Minus;
        case TokenType::Multiply:
            return UnaryOperators::Indirection;
        case TokenType::BitAnd:
            return UnaryOperators::Address;
        case TokenType::Tilde:
            return UnaryOperators::BitwiseNot;
        case TokenType::Bang:
            return UnaryOperators::LogicalNot;
        default:
            return std::nullopt;
        }
    }
} // namespace Marble
