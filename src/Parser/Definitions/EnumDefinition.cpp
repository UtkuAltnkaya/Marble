#include "Ast/Definitions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{

    EnumDefinition::EnumDefinition(AccessSpecifier accessSpecifier, Box<Identifier> enumName, std::vector<Box<Identifier>> &&fields, const Span &span)
        : Definition{span, DefinitionType::Enum}, m_AccessSpecifier{accessSpecifier}, m_EnumName{std::move(enumName)}, m_Fields{std::move(fields)}, m_ImplDefinition{nullptr}
    {
    }

    Box<Definition> EnumDefinition::Parse(Parser &parser, AccessSpecifier accessSpecifier, const Span &span)
    {
        Span start = accessSpecifier == AccessSpecifier::Public ? span : parser.Current().Span();
        Box<Identifier> enumName = Identifier::Parse(parser);

        parser.NextTokenAndExpect(TokenType::OpenCurlyBrace);

        std::vector<Box<Identifier>> fields;
        Parenthesis::Parse<Box<Identifier>>(fields, parser, TokenType::CloseCurlyBrace, [](Parser &parser)
                                            { return MakeBox<Identifier>(parser.Expect(TokenType::Identifier)); });

        const Span &end = parser.Current().Span();
        Box<EnumDefinition> enumDefinition = MakeBox<EnumDefinition>(
            accessSpecifier, std::move(enumName), std::move(fields), Span{start.Start, end.Start});

        SymbolTable::Get().Insert(*enumDefinition);

        return enumDefinition;
    }
} // namespace Marble
