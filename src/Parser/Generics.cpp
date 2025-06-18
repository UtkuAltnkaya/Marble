#include "Ast/Generics.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"

namespace Marble
{
    Generics::Generics(std::vector<Ref<TypeSpecifier>> &&types, const Span &span)
        : Ast{span, AstType::Generics}, m_Types{std::move(types)}
    {
    }

    Generics::Generics(const Generics &obj) : Ast{obj.m_Span, AstType::Generics}
    {
        for (auto &type : obj.m_Types)
        {
            m_Types.push_back(type);
        }
        }

    // To prevent ambiguity between '<' operator and '<' generic greedy parsing strategy used
    // TODO: Search for better parsing strategy
    Box<Generics> Generics::Parse(Parser &parser)
    {
        if (parser.Next().TokenType() != TokenType::LessThan)
        {
            return nullptr;
        }

        parser.CreateCheckpoint();
        Span start = parser.Current().Span();

        // Skip '<' token
        parser.NextToken();

        std::vector<Ref<TypeSpecifier>> types;

        try
        {
            Parenthesis::Parse<Ref<TypeSpecifier>>(types, parser, TokenType::GreaterThan, [](Parser &parser)
                                                   { return TypeSpecifier::Parse(parser); });
        }
        catch (...)
        {
            parser.RollBack();
            return nullptr;
        }

        parser.DiscardCheckpoint();
        Span span = Span{start.Start, parser.Current().Span().Start};
        return MakeBox<Generics>(std::move(types), span);
    }

} // namespace Marble
