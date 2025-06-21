#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"

namespace Marble
{
    ObjectInitExpression::ObjectInitExpression(Box<Expression> object, Box<Generics> generics, std::vector<Box<Expression>> &&fields, const Span &span)
        : Expression{span, ExpressionType::ObjectInit}, m_Object{std::move(object)}, m_Generics{std::move(generics)}, m_Fields{std::move(fields)}
    {
    }

    ObjectInitExpression::ObjectInitExpression(const ObjectInitExpression &obj) : Expression{obj.m_Span, ExpressionType::ObjectInit}
    {
        m_Object = obj.m_Object->Clone();
        m_Generics = MakeBox<Generics>(*obj.m_Generics.get());
        for (auto &field : obj.m_Fields)
        {
            m_Fields.push_back(field->Clone());
        }
    }

    Box<Expression> ObjectInitExpression::Parse(Parser &parser, Precedence precedence)
    {
        Box<Expression> left = Expression::Parse(parser, Expression::NextPrecedence(precedence));

        Box<Generics> generics = Generics::Parse(parser);

        if (parser.Next().TokenType() != TokenType::OpenCurlyBrace)
        {
            return left;
        }

        const Span &start = left->GetSpan();

        parser.NextToken();

        std::vector<Box<Expression>> fields;
        Parenthesis::Parse<Box<Expression>>(fields, parser, TokenType::CloseCurlyBrace, [](Parser &parser)
                                            { return FieldExpression::Parse(parser); });

        const Span &end = parser.Current().Span();
        Span span{start.Start, end.End};
        return MakeBox<ObjectInitExpression>(std::move(left), std::move(generics), std::move(fields), span);
    }

    FieldExpression::FieldExpression(Identifier &&name, Box<Expression> value, const Span &span)
        : Expression{span, ExpressionType::ObjectField}, m_Name{std::move(name)}, m_Value{std::move(value)}
    {
    }

    FieldExpression::FieldExpression(const FieldExpression &obj)
        : Expression{obj.m_Span, ExpressionType::ObjectField}, m_Name{obj.m_Name}
    {
        m_Value = obj.m_Value->Clone();
    }

    Box<Expression> FieldExpression::Parse(Parser &parser)
    {
        Identifier name{parser.Expect(TokenType::Identifier)};
        TokenType tokenType = parser.Next().TokenType();

        const Span &start = name.GetSpan();
        if (tokenType == TokenType::Comma || tokenType == TokenType::CloseCurlyBrace)
        {
            Identifier temp{name};
            Box<Expression> idExpr = MakeBox<IdentifierExpression>(std::move(temp), start);
            return MakeBox<FieldExpression>(std::move(name), std::move(idExpr), start);
        }

        parser.NextTokenAndExpect(TokenType::Colon);
        parser.NextToken();

        Box<Expression> value = Expression::Parse(parser);

        const Span &end = value->GetSpan();
        Span span{start.Start, end.End};
        return MakeBox<FieldExpression>(std::move(name), std::move(value), span);
    }
} // namespace Marble
