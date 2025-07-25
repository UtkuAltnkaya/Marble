#include "Parser/Parser.hpp"

#include "Ast/Expressions.hpp"

namespace Marble
{

    IdentifierExpression::IdentifierExpression(const Identifier &identifier, const Span &span)
        : Expression{span, ExpressionType::Identifier}, m_Identifier{identifier}
    {
    }

    IdentifierExpression::IdentifierExpression(Identifier &&identifier, Span &&span)
        : Expression{std::move(span), ExpressionType::Identifier}, m_Identifier{std::move(identifier)}
    {
    }

    IdentifierExpression::IdentifierExpression(const IdentifierExpression &obj)
        : Expression{obj.m_Span, ExpressionType::Identifier}, m_Identifier{obj.m_Identifier}
    {
        if (obj.m_ValueType)
        {
            m_ValueType = MakeRef<TypeSpecifier>(*obj.m_ValueType.get());
        }
    }

    Box<Expression> IdentifierExpression::Parse(Parser &parser, Precedence precedence)
    {
        const Token &token = parser.Current();
        if (token.TokenType() != TokenType::Identifier)
        {
            return Expression::Parse(parser, Expression::NextPrecedence(precedence));
        }
        return std::make_unique<IdentifierExpression>(Identifier{token.Text(), token.Span()}, token.Span());
    }
} // namespace Marble
