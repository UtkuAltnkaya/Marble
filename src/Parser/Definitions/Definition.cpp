#include "Ast/Definitions.hpp"
#include "Ast/AccessSpecifier.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    Definition::Definition(const Span &span, Marble::DefinitionType definitionType)
        : Ast{span, AstType::Definition}, m_DefinitionType{definitionType}
    {
    }

    Definition::Definition(Span &&span, Marble::DefinitionType definitionType)
        : Ast{std::move(span), AstType::Definition}, m_DefinitionType{definitionType}
    {
    }

    Box<Definition> Definition::Parse(Parser &parser)
    {
        Span start;
        AccessSpecifier accessSpecifier = ParseAccessSpecifier(parser, start);

        switch (parser.Current().TokenType())
        {
        case TokenType::Fn:
            return FunctionDefinition::Parse(parser, accessSpecifier, start);
        case TokenType::Struct:
            return StructDefinition::Parse(parser, accessSpecifier, start);
        case TokenType::Enum:
            return EnumDefinition::Parse(parser, accessSpecifier, start);
        case TokenType::Impl:
            if (accessSpecifier == AccessSpecifier::Public)
            {
                throw "Unexpected access specifier";
                // throw SyntacticError(parser, "Unexpected access specifier");
            }
            return ImplDefinition::Parse(parser);
        default:
            // return SyntacticError(parser, "Unknown definition");
            throw "Unknown definition";
        }
    }
} // namespace Marble
