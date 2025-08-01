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

        bool result = Parenthesis::Parse<Ref<TypeSpecifier>>(types, parser, TokenType::GreaterThan, [](Parser &parser)
                                                             { return TypeSpecifier::Parse(parser); });
        if (!result)
        {
            ErrorSystem::RemoveLastError();
            parser.RollBack();
            return nullptr;
        }

        parser.DiscardCheckpoint();
        Span span = Span{start.Start, parser.Current().Span().Start};
        return MakeBox<Generics>(std::move(types), span);
    }

    std::unordered_map<std::string, Ref<TypeSpecifier>> Generics::ToMap(const std::vector<Ref<TypeSpecifier>> &typeArgs) const
    {
        if (m_Types.size() != typeArgs.size())
        {
            throw "Generic types does not matches";
        }

        std::unordered_map<std::string, Ref<TypeSpecifier>> map;

        for (size_t i = 0; i < m_Types.size(); i++)
        {
            if (m_Types[i]->GetType() != Types::UserDefine)
            {
                throw "Type arguments must be user define type";
            }
            const Identifier &id = m_Types[i]->UserDefine().Type;
            map[id.Id()] = typeArgs[i];
        }
        return map;
    }

    void Generics::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        for (auto &type : m_Types)
        {
            type->SubstituteGenerics(semanticAnalyzer, map);
        }
    }
} // namespace Marble
