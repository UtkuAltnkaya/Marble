#include "Ast/VariableType.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    VariableType::VariableType(Box<Identifier> identifier, Ref<TypeSpecifier> typeSpecifier, const Span &span)
        : Ast{span, AstType::VariableType}, m_Identifier{std::move(identifier)}, m_TypeSpecifier{typeSpecifier}
    {
    }

    Box<VariableType> VariableType::Parse(Parser &parser)
    {
        Box<Identifier> identifier = MakeBox<Identifier>(parser.Expect(TokenType::Identifier));
        Span start = parser.Current().Span();
        parser.NextTokenAndExpect(TokenType::Colon);
        parser.NextToken();
        Ref<TypeSpecifier> typeSpecifier = TypeSpecifier::Parse(parser);
        const Span &end = parser.Current().Span();
        return MakeBox<VariableType>(std::move(identifier), typeSpecifier, Span{start.Start, end.End});
    }

} // namespace Marble
