#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    PrimitiveExpression::PrimitiveExpression(Ref<TypeSpecifier> typeSpecifier, const std::string &value, const Span &span)
        : Expression{span, ExpressionType::Primitive}, m_TypeSpecifier{typeSpecifier}, m_Value{value}
    {
    }
    PrimitiveExpression::PrimitiveExpression(Ref<TypeSpecifier> typeSpecifier, std::string &&value, Span &&span)
        : Expression{std::move(span), ExpressionType::Primitive}, m_TypeSpecifier{typeSpecifier}, m_Value{std::move(value)}
    {
    }

    PrimitiveExpression::PrimitiveExpression(const PrimitiveExpression &obj) : Expression{obj.m_Span, ExpressionType::Primitive}
    {
        m_TypeSpecifier = MakeRef<TypeSpecifier>(*obj.m_TypeSpecifier.get());
        m_Value = obj.m_Value;
    }

    Box<Expression> PrimitiveExpression::Parse(Parser &parser, Precedence precedence)
    {
        if (auto expr = PrimitiveExpression::ParseParenthesis(parser); expr)
        {
            return expr;
        }

        const Token &token = parser.Current();

        Ref<TypeSpecifier> typeSpecifier = MakeRef<TypeSpecifier>(Types::Void);
        switch (token.TokenType())
        {
        case TokenType::Number:
            if (token.Text().find_first_of('.') != std::string::npos)
            {
                typeSpecifier->SetType(Types::Float);
                break;
            }
            typeSpecifier->SetType(Types::Int);
            break;
        case TokenType::String:
            typeSpecifier->SetType(Types::Str);
            break;
        case TokenType::Char:
            typeSpecifier->SetType(Types::Char);
            break;
        case TokenType::Null:
            // TODO
            typeSpecifier->SetType(Types::Null);
            break;
        case TokenType::True:
        case TokenType::False:
            typeSpecifier->SetType(Types::Bool);
            break;
        default:
            return Expression::Parse(parser, Expression::NextPrecedence(precedence));
        }

        return MakeBox<PrimitiveExpression>(typeSpecifier, token.Text(), token.Span());
    }

    Box<Expression> PrimitiveExpression::ParseParenthesis(Parser &parser)
    {
        if (parser.Current().TokenType() != TokenType::OpenParen)
        {
            return nullptr;
        }

        parser.NextToken();
        Box<Expression> expr = Expression::Parse(parser);
        parser.NextTokenAndExpect(TokenType::CloseParen);
        return expr;
    }

    Box<Expression> PrimitiveExpression::Clone()
    {
        return MakeBox<PrimitiveExpression>(*this);
    }
} // namespace Marble
