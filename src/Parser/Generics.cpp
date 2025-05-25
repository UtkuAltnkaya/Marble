#include "Ast/Generics.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"

namespace Marble
{
    Generics::Generics(std::vector<Ref<TypeSpecifier>> &&types, const Span &span)
        : Ast{span, AstType::Generics}, m_Types{std::move(types)}
    {
    }

    Box<Generics> Generics::Parse(Parser &parser)
    {
        if (parser.Next().TokenType() != TokenType::LessThan)
        {
            return nullptr;
        }
        Span start = parser.Current().Span();

        // Skip '<' token
        parser.NextToken();

        std::vector<Ref<TypeSpecifier>> types;
        Parenthesis::Parse<Ref<TypeSpecifier>>(types, parser, TokenType::GreaterThan, [](Parser &parser)
                                               { return TypeSpecifier::Parse(parser); });

        Span span = Span{start.Start, parser.Current().Span().Start};
        return MakeBox<Generics>(std::move(types), span);
    }

} // namespace Marble
