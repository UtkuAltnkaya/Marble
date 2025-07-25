#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    NamespaceExpression::NamespaceExpression(Box<Expression> namespaceExpr, Box<Generics> generics, Box<Expression> value, const Span &span)
        : Expression{span, ExpressionType::NameSpace},
          m_Namespace{std::move(namespaceExpr)}, m_Generics{std::move(generics)}, m_Value{std::move(value)}
    {
    }

    NamespaceExpression::NamespaceExpression(const NamespaceExpression &obj) : Expression{obj.m_Span, ExpressionType::NameSpace}
    {
        m_Namespace = obj.m_Namespace->Clone();
        m_Generics = obj.m_Generics ? MakeBox<Generics>(*obj.m_Generics.get()) : nullptr;
        m_Value = obj.m_Value->Clone();

        if (obj.m_ValueType)
        {
            m_ValueType = MakeRef<TypeSpecifier>(*obj.m_ValueType.get());
        }
    }

    Box<Expression> NamespaceExpression::Parse(Parser &parser, Precedence precedence)
    {
        Box<Expression> left = Expression::Parse(parser, Expression::NextPrecedence(precedence));

        parser.CreateCheckpoint();
        Box<Generics> generics = Generics::Parse(parser);

        if (parser.Next().TokenType() != TokenType::Colon)
        {
            if (generics)
            {
                parser.RollBack();
            }
            parser.DiscardCheckpoint();
            return left;
        }
        parser.DiscardCheckpoint();
        const Span &start = left->GetSpan();
        parser.NextToken(); // Skip first colon
        parser.NextTokenAndExpect(TokenType::Colon);
        parser.NextToken(); // Skip second colon

        Box<Expression> value = Expression::Parse(parser, Precedence::FunctionCall);
        const Span &end = value->GetSpan();
        Span span{start.Start, end.End};
        return MakeBox<NamespaceExpression>(std::move(left), std::move(generics), std::move(value), span);
    }
} // namespace Marble
