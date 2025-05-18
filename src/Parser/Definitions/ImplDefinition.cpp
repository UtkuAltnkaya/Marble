#include "Ast/Definitions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"
#include "Definitions.hpp"

namespace Marble
{

    ImplDefinition::ImplDefinition(Ref<TypeSpecifier> implName, std::vector<Box<MemberFunctionDefinition>> &&memberFunctions, const Span &span)
        : Definition{span, DefinitionType::Impl}, m_ImplName{implName}, m_MemberFunctions{std::move(m_MemberFunctions)}
    {
    }

    Box<Definition> ImplDefinition::Parse(Parser &parser)
    {
        Span start = parser.Current().Span();

        // Skip impl token;
        parser.NextToken();

        Ref<TypeSpecifier> implName = TypeSpecifier::Parse(parser);

        std::vector<Box<MemberFunctionDefinition>> memberFunctions;
        Parenthesis::Parse<Box<MemberFunctionDefinition>>(memberFunctions, parser, TokenType::CloseCurlyBrace, [](Parser &parser)
                                                          { return MemberFunctionDefinition::Parse(parser); });

        const Span &end = parser.Current().Span();

        Box<ImplDefinition> implDefinition = MakeBox<ImplDefinition>(implName, std::move(memberFunctions), Span{start.Start, end.Start});
    }

    MemberFunctionDefinition::MemberFunctionDefinition(Box<MemberFunctionPrototypeDefinition> prototype, Box<Statement> block, const Span &span)
        : Definition{span, DefinitionType::MemberFunction}, m_Prototype{std::move(prototype)}, m_Block{std::move(block)}
    {
    }

    Box<MemberFunctionDefinition> MemberFunctionDefinition::Parse(Parser &parser)
    {
        Box<MemberFunctionPrototypeDefinition> prototype = MemberFunctionPrototypeDefinition::Parse(parser);
        Box<Statement> block = BlockStatement::Parse(parser);
        Span start = prototype->GetSpan();
        const Span &end = parser.Current().Span();
        return MakeBox<MemberFunctionDefinition>(std::move(prototype), std::move(block), Span{start.Start, end.Start});
    }

    MemberFunctionPrototypeDefinition::MemberFunctionPrototypeDefinition(
        AccessSpecifier accessSpecifier,
        Box<VariableType> method,
        Box<Identifier> name,
        std::vector<Box<VariableType>> &&params,
        Ref<TypeSpecifier> returnType,
        const Span &span)
        : Definition{span, DefinitionType::MemberFunctionPrototype}, m_AccessSpecifier{accessSpecifier}, m_Method{std::move(method)},
          m_Name{std::move(name)}, m_Params{std::move(params)}, m_ReturnType{returnType}
    {
    }

    Box<MemberFunctionPrototypeDefinition> MemberFunctionPrototypeDefinition::Parse(Parser &parser)
    {
        Span span;
        AccessSpecifier accessSpecifier = ParseAccessSpecifier(parser, span);

        Span start = accessSpecifier == AccessSpecifier::Public ? span : parser.Current().Span();

        Box<VariableType> method = MemberFunctionPrototypeDefinition::ParseMethod(parser);

        Box<Identifier> name = Identifier::Parse(parser);

        // Params
        std::vector<Box<VariableType>> params;
        parser.NextTokenAndExpect(TokenType::OpenParen);
        Parenthesis::Parse<Box<VariableType>>(params, parser, TokenType::CloseParen, [](Parser &parser)
                                              { return VariableType::Parse(parser); });

        // Return type
        parser.NextTokenAndExpect(TokenType::Arrow);
        parser.NextToken();
        Ref<TypeSpecifier> returnType = TypeSpecifier::Parse(parser);

        const Span &end = parser.Current().Span();

        return MakeBox<MemberFunctionPrototypeDefinition>(
            accessSpecifier,
            std::move(method),
            std::move(name),
            std::move(params),
            std::move(returnType),
            Span{start.Start, end.End});
    }

    Box<VariableType> MemberFunctionPrototypeDefinition::ParseMethod(Parser &parser)
    {

        if (parser.Next().TokenType() != TokenType::OpenParen)
        {
            return nullptr;
        }

        // Skip fn keyword
        parser.NextToken();
        // Skip open parenthesis
        parser.NextToken();

        Box<VariableType> variableType = VariableType::Parse(parser);
        parser.NextTokenAndExpect(TokenType::CloseParen);
        return variableType;
    }
} // namespace Marble
