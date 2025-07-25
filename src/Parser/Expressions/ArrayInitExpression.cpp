#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"

namespace Marble
{
    ArrayInitExpression::ArrayInitExpression(std::vector<Box<Expression>> &&array, size_t size, const Span &span)
        : Expression{span, ExpressionType::ArrayInit}, m_Array{std::move(array)}, m_Size{size}
    {
    }

    ArrayInitExpression::ArrayInitExpression(const ArrayInitExpression &obj) : Expression{obj.m_Span, ExpressionType::ArrayInit}
    {
        for (auto &array : obj.m_Array)
        {
            m_Array.push_back(array->Clone());
        }
        m_Size = obj.m_Size;
        if (obj.m_ValueType)
        {
            m_ValueType = MakeRef<TypeSpecifier>(*obj.m_ValueType.get());
        }
    }

    Box<Expression> ArrayInitExpression::Parse(Parser &parser, Precedence precedence)
    {
        if (parser.Current().TokenType() != TokenType::OpenBracket)
        {
            return Expression::Parse(parser, Expression::NextPrecedence(precedence));
        }

        Span start = parser.Current().Span();

        std::vector<Box<Expression>> array;
        Parenthesis::Parse<Box<Expression>>(array, parser, TokenType::CloseBracket, [](Parser &parser)
                                            { return Expression::Parse(parser); });
        size_t size = array.size();
        const Span &end = parser.Current().Span();
        return MakeBox<ArrayInitExpression>(std::move(array), size, Span{start.Start, end.End});
    }
} // namespace Marble
