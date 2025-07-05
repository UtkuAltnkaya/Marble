#include "ErrorSystem/ErrorSystem.hpp"
#include "Ast/Definitions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"
namespace Marble
{

    ImplDefinition::ImplDefinition(Ref<TypeSpecifier> implName, Box<Generics> generics, std::vector<Box<MemberFunctionDefinition>> &&memberFunctions, const Span &span)
        : Definition{span, DefinitionType::Impl}, m_ImplName{implName},
          m_Generics{std::move(generics)}, m_MemberFunctions{std::move(memberFunctions)}
    {
    }

    ImplDefinition::ImplDefinition(const ImplDefinition &obj) : Definition{obj.m_Span, DefinitionType::Impl}
    {
        m_ImplName = MakeRef<TypeSpecifier>(*obj.m_ImplName.get());
        m_Generics = MakeBox<Generics>(*obj.m_Generics.get());
        m_MemberFunctions.reserve(obj.m_MemberFunctions.size());

        for (auto &memberFunction : obj.m_MemberFunctions)
        {
            m_MemberFunctions.push_back(MakeBox<MemberFunctionDefinition>(*memberFunction.get()));
        }
    }

    Box<Definition> ImplDefinition::Parse(Parser &parser)
    {
        Span start = parser.Current().Span();

        // Skip impl token;
        parser.NextToken();

        Ref<TypeSpecifier> implName = TypeSpecifier::Parse(parser);

        Box<Generics> generics = nullptr;
        if (implName->GetType() == Types::GenericType)
        {
            auto genericType = implName->Generic();
            Identifier &identifier = genericType.OuterType;
            Span span = Span{identifier.GetSpan().End, implName->GetSpan().End};
            generics = MakeBox<Generics>(std::move(genericType.InnerType), span);
            implName = MakeRef<TypeSpecifier>(identifier, identifier.GetSpan());
        }

        std::vector<Box<MemberFunctionDefinition>> memberFunctions;
        parser.NextTokenAndExpect(TokenType::OpenCurlyBrace);
        do
        {
            parser.NextToken();
            if (parser.Current().TokenType() == TokenType::CloseCurlyBrace)
            {
                break;
            }
            if (parser.Current().TokenType() == TokenType::Eof)
            {
                ErrorSystem::AddError(parser, "Missing curly brace '}'");
            }
            memberFunctions.push_back(MemberFunctionDefinition::Parse(parser));
        } while (true);

        const Span &end = parser.Current().Span();

        Box<ImplDefinition> implDefinition = MakeBox<ImplDefinition>(
            implName, std::move(generics), std::move(memberFunctions), Span{start.Start, end.Start});

        implDefinition->CreateSymbol();

        return implDefinition;
    }

    void ImplDefinition::AddMemberFunction(Box<MemberFunctionDefinition> memberFunction)
    {
        m_MemberFunctions.push_back(std::move(memberFunction));
    }

    MemberFunctionDefinition::MemberFunctionDefinition(Box<MemberFunctionPrototypeDefinition> prototype, Box<Statement> block, const Span &span)
        : Definition{span, DefinitionType::MemberFunction}, m_Prototype{std::move(prototype)}, m_Block{std::move(block)}
    {
    }
    MemberFunctionDefinition::MemberFunctionDefinition(const MemberFunctionDefinition &obj)
        : Definition{obj.m_Span, DefinitionType::MemberFunction}
    {
        m_Prototype = MakeBox<MemberFunctionPrototypeDefinition>(*obj.m_Prototype.get());
        m_Block = obj.m_Block->Clone();
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
        Box<Generics> generics,
        std::vector<Box<VariableType>> &&params,
        Ref<TypeSpecifier> returnType,
        const Span &span)
        : Definition{span, DefinitionType::MemberFunctionPrototype}, m_AccessSpecifier{accessSpecifier}, m_Method{std::move(method)},
          m_Name{std::move(name)}, m_Generics{std::move(generics)}, m_Params{std::move(params)}, m_ReturnType{returnType}
    {
    }

    MemberFunctionPrototypeDefinition::MemberFunctionPrototypeDefinition(const MemberFunctionPrototypeDefinition &obj)
        : Definition{obj.m_Span, DefinitionType::MemberFunctionPrototype}
    {
        m_AccessSpecifier = obj.m_AccessSpecifier;
        m_Method = obj.m_Method ? MakeBox<VariableType>(*obj.m_Method.get()) : nullptr;
        m_Name = MakeBox<Identifier>(*obj.m_Name.get());
        m_Generics = obj.m_Generics ? MakeBox<Generics>(*obj.m_Generics.get()) : nullptr;
        m_Params.reserve(obj.m_Params.size());
        for (auto &param : obj.m_Params)
        {
            m_Params.push_back(MakeBox<VariableType>(*param.get()));
        }
        m_ReturnType = MakeRef<TypeSpecifier>(*obj.m_ReturnType.get());
    }

    Box<MemberFunctionPrototypeDefinition> MemberFunctionPrototypeDefinition::Parse(Parser &parser)
    {
        Span span;
        AccessSpecifier accessSpecifier = ParseAccessSpecifier(parser, span);

        Span start = accessSpecifier == AccessSpecifier::Public ? span : parser.Current().Span();

        parser.Expect(TokenType::Fn);
        parser.NextToken();

        Box<VariableType> method = MemberFunctionPrototypeDefinition::ParseMethod(parser);

        Box<Identifier> name = MakeBox<Identifier>(parser.Expect(TokenType::Identifier));

        Box<Generics> generics = Generics::Parse(parser);

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
            std::move(generics),
            std::move(params),
            std::move(returnType),
            Span{start.Start, end.End});
    }

    Box<VariableType> MemberFunctionPrototypeDefinition::ParseMethod(Parser &parser)
    {
        if (parser.Current().TokenType() != TokenType::OpenParen)
        {
            return nullptr;
        }

        // Skip open parenthesis
        parser.NextToken();

        Box<VariableType> variableType = VariableType::Parse(parser);
        parser.NextTokenAndExpect(TokenType::CloseParen);

        // Skip close parenthesis
        parser.NextToken();
        return variableType;
    }
} // namespace Marble
