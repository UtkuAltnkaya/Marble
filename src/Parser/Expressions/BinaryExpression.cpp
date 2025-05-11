#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    BinaryExpression::BinaryExpression(
        Box<Expression> left, BinaryOperators binaryOperator, Box<Expression> right, const Span &span)
        : Expression{span, ExpressionType::Binary}, m_Left{std::move(left)}, m_Operator{binaryOperator}, m_Right{std::move(right)}
    {
    }

    BinaryExpression::BinaryExpression(
        Box<Expression> left, BinaryOperators binaryOperator, Box<Expression> right, Span &&span)
        : Expression{std::move(span), ExpressionType::Binary}, m_Left{std::move(left)}, m_Operator{binaryOperator}, m_Right{std::move(right)}

    {
    }

    Box<Expression> BinaryExpression::Parse(Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence)
    {

        switch (binaryPrecedence)
        {
        case BinaryPrecedence::Or:
            return BinaryExpression::ParseOr(parser, precedence, binaryPrecedence);

        case BinaryPrecedence::And:
            return BinaryExpression::ParseExpression(parser, precedence, binaryPrecedence, [](TokenType token_type)
                                                     { return token_type == TokenType::And; });
        case BinaryPrecedence::BitOr:
            return BinaryExpression::ParseExpression(parser, precedence, binaryPrecedence, [](TokenType token_type)
                                                     { return token_type == TokenType::BitOr; });
        case BinaryPrecedence::BitXor:
            return BinaryExpression::ParseExpression(parser, precedence, binaryPrecedence, [](TokenType token_type)
                                                     { return token_type == TokenType::BitXor; });
        case BinaryPrecedence::BitAnd:
            return BinaryExpression::ParseExpression(parser, precedence, binaryPrecedence, [](TokenType token_type)
                                                     { return token_type == TokenType::BitAnd; });
        case BinaryPrecedence::BitLeft:
            return BinaryExpression::ParseExpression(parser, precedence, binaryPrecedence, [](TokenType token_type)
                                                     { return token_type == TokenType::BitLeft; });
        case BinaryPrecedence::BitRight:
            return BinaryExpression::ParseExpression(parser, precedence, binaryPrecedence, [](TokenType token_type)
                                                     { return token_type == TokenType::BitRight; });
        case BinaryPrecedence::Equal:
            return BinaryExpression::ParseExpression(parser, precedence, binaryPrecedence, [](TokenType token_type)
                                                     { return token_type == TokenType::Equal || token_type == TokenType::NotEqual; });
        case BinaryPrecedence::Relational:
            return BinaryExpression::ParseExpression(parser, precedence, binaryPrecedence, [](TokenType token_type)
                                                     { return token_type == TokenType::LessThan ||
                                                              token_type == TokenType::LessEqual ||
                                                              token_type == TokenType::GreaterThan ||
                                                              token_type == TokenType::GreaterEqual; });
        case BinaryPrecedence::Sum:
            return BinaryExpression::ParseExpression(parser, precedence, binaryPrecedence, [](TokenType token_type)
                                                     { return token_type == TokenType::Plus || token_type == TokenType::Minus; });
        case BinaryPrecedence::Product:
            return BinaryExpression::ParseProduct(parser, precedence, binaryPrecedence);
        default:
            return nullptr;
        }
    }

    Box<Expression> BinaryExpression::ParseOr(
        Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence)
    {
        Box<Expression> left{BinaryExpression::Parse(parser, precedence, BinaryExpression::NextPrecedence(binaryPrecedence))};

        if (parser.Next().TokenType() != TokenType::Or)
        {
            return left;
        }

        const Span start = left->GetSpan();

        parser.NextToken();
        BinaryOperators op = BinaryExpression::StrToOperator(parser.Current().Text().c_str());
        parser.NextToken();
        Box<Expression> right{BinaryExpression::Parse(parser, precedence, binaryPrecedence)};
        const Span end = right->GetSpan();

        return MakeBox<BinaryExpression>(std::move(left), op, std::move(right), Span{start.Start, end.End});
    }

    Box<Expression> BinaryExpression::ParseExpression(
        Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence, std::function<bool(TokenType)> condition)
    {
        Box<Expression> left{BinaryExpression::Parse(parser, precedence, BinaryExpression::NextPrecedence(binaryPrecedence))};

        if (!condition(parser.Next().TokenType()))
        {
            return left;
        }

        const Span start = left->GetSpan();

        parser.NextToken();
        BinaryOperators op = BinaryExpression::StrToOperator(parser.Current().Text().c_str());
        parser.NextToken();
        Box<Expression> right{BinaryExpression::Parse(parser, precedence, binaryPrecedence)};
        const Span end = right->GetSpan();

        return MakeBox<BinaryExpression>(std::move(left), op, std::move(right), Span{start.Start, end.End});
    }

    Box<Expression> BinaryExpression::ParseProduct(
        Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence)
    {
        Box<Expression> left{Expression::Parse(parser, Expression::NextPrecedence(precedence))};

        TokenType tokenType = parser.Next().TokenType();
        if (tokenType != TokenType::Multiply && tokenType != TokenType::Divide && tokenType != TokenType::Percent)
        {
            return left;
        }

        const Span start = left->GetSpan();

        parser.NextToken();
        BinaryOperators op = BinaryExpression::StrToOperator(parser.Current().Text().c_str());
        parser.NextToken();
        Box<Expression> right{BinaryExpression::Parse(parser, precedence, binaryPrecedence)};
        const Span end = right->GetSpan();

        return MakeBox<BinaryExpression>(std::move(left), op, std::move(right), Span{start.Start, end.End});
    }

    BinaryPrecedence BinaryExpression::NextPrecedence(BinaryPrecedence binaryPrecedence)
    {
        if ((int)binaryPrecedence < (int)BinaryPrecedence::END - (int)BinaryPrecedence::START - 1)
        {
            return (BinaryPrecedence)((int)binaryPrecedence + 1);
        }
        return (BinaryPrecedence)((int)BinaryPrecedence::END - 1);
    }

    BinaryOperators Marble::BinaryExpression::StrToOperator(const char *text)
    {
        static std::unordered_map<std::string, BinaryOperators> map{
            {"+", BinaryOperators::Add},
            {"-", BinaryOperators::Subtract},
            {"*", BinaryOperators::Multiply},
            {"/", BinaryOperators::Divide},
            {"%", BinaryOperators::Modulo},
            {"==", BinaryOperators::Equal},
            {"!=", BinaryOperators::NotEqual},
            {"&&", BinaryOperators::And},
            {"||", BinaryOperators::Or},
            {"&", BinaryOperators::BitAnd},
            {"|", BinaryOperators::BitOr},
            {"^", BinaryOperators::BitXor},
            {"<", BinaryOperators::LessThan},
            {">", BinaryOperators::GreaterThan},
            {"<=", BinaryOperators::LessThanOrEqual},
            {">=", BinaryOperators::GreaterThanOrEqual},
            {">>", BinaryOperators::BitRight},
            {"<<", BinaryOperators::BitLeft},
        };
        return map[text];
    }
} // namespace Marble
