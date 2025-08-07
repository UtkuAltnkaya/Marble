#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    MemberAccessExpression::MemberAccessExpression(Box<Expression> object, TokenType accessType, Box<Expression> property, const Span &span)
        : Expression{span, ExpressionType::MemberAccess}, m_Object{std::move(object)}, m_AccessType{accessType}, m_Property{std::move(property)}
    {
    }

    MemberAccessExpression::MemberAccessExpression(const MemberAccessExpression &obj) : Expression{obj.m_Span, ExpressionType::MemberAccess}
    {
        m_Object = obj.m_Object->Clone();
        m_AccessType = obj.m_AccessType;
        m_Property = obj.m_Property->Clone();

        if (obj.m_ValueType)
        {
            m_ValueType = MakeRef<TypeSpecifier>(*obj.m_ValueType.get());
        }
    }

    Box<Expression> MemberAccessExpression::Parse(Parser &parser, Precedence precedence)
    {

        Box<Expression> left{Expression::Parse(parser, Expression::NextPrecedence(precedence))};
        while (true)
        {
            TokenType token_type = parser.Next().TokenType();

            if (token_type != TokenType::Dot && token_type != TokenType::Arrow)
            {
                break;
            }

            const Span &start = left->GetSpan();
            parser.NextToken();
            parser.NextToken();

            Box<Expression> property{Expression::Parse(parser, Expression::NextPrecedence(precedence))};

            const Span &end = property->GetSpan();
            Span span{start.Start, end.End};

            left = MakeBox<MemberAccessExpression>(std::move(left), token_type, std::move(property), span);
        }
        return left;
    }
} // namespace Marble
