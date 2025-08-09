#include "Ast/Definitions.hpp"
#include "Ast/AccessSpecifier.hpp"
#include "Parser/Parser.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    Definition::Definition(const Span &span, Marble::DefinitionType definitionType)
        : Ast{span, AstType::Definition}, m_DefinitionType{definitionType}, m_IsExpanded{false}
    {
    }

    Definition::Definition(Span &&span, Marble::DefinitionType definitionType)
        : Ast{std::move(span), AstType::Definition}, m_DefinitionType{definitionType}, m_IsExpanded{false}
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
                ErrorSystem::AddError(parser, "Unexpected access specifier");
            }
            return ImplDefinition::Parse(parser);
        default:
            ErrorSystem::AddError(parser, "Unexpected definition");
        }
        return nullptr;
    }
} // namespace Marble
