#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/Conversion.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    CastExpression::CastExpression(Ref<TypeSpecifier> typeSpecifier, Box<Expression> expression, ConversionKind kind, const Span &span)
        : Expression{span, ExpressionType::Cast}, m_TypeSpecifier{typeSpecifier}, m_Expression{std::move(expression)}, m_Kind{kind}
    {
    }

    CastExpression::CastExpression(const CastExpression &obj) : Expression{obj.m_Span, ExpressionType::Cast}
    {
        m_Kind = obj.m_Kind;
        m_TypeSpecifier = MakeRef<TypeSpecifier>(*obj.m_TypeSpecifier.get());
        m_Expression = obj.m_Expression->Clone();
        if (obj.m_ValueType)
        {
            m_ValueType = MakeRef<TypeSpecifier>(*obj.m_ValueType.get());
        }
    }

    CastExpression::CastExpression(Ref<TypeSpecifier> typeSpecifier, Box<Expression> expression, ConversionKind kind, Span &&span)
        : Expression{std::move(span), ExpressionType::Cast}, m_TypeSpecifier{typeSpecifier}, m_Expression{std::move(expression)}, m_Kind{kind}
    {
    }

    Box<Expression> CastExpression::Parse(Parser &parser, Precedence precedence)
    {
        Box<Expression> left = Expression::Parse(parser, Expression::NextPrecedence(precedence));
        if (parser.Next().TokenType() != TokenType::As)
        {
            return left;
        }

        const Span &start = left->GetSpan();
        parser.NextToken();
        parser.NextToken(); // Skip as token

        Ref<TypeSpecifier> typeSpecifier = TypeSpecifier::Parse(parser);

        const Span &end = typeSpecifier->GetSpan();
        Span span{start.Start, end.End};
        return MakeBox<CastExpression>(typeSpecifier, std::move(left), ConversionKind::None, span);
    }

} // namespace Marble
