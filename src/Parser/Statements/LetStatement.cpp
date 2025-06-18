#include "Ast/Statements.hpp"
#include "Parser/Parser.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{

    LetStatement::LetStatement(Box<Identifier> identifier, Ref<TypeSpecifier> typeSpecifier, Box<Expression> value, const Span &span)
        : Statement{span, StatementType::Let}, m_Identifier{std::move(identifier)}, m_TypeSpecifier{typeSpecifier}, m_Value{std::move(value)}
    {
    }

    LetStatement::LetStatement(const LetStatement &obj) : Statement{obj.m_Span, StatementType::Let}
    {
        m_Identifier = MakeBox<Identifier>(*obj.m_Identifier.get());

        if (obj.m_TypeSpecifier)
        {
            m_TypeSpecifier = MakeRef<TypeSpecifier>(*obj.m_TypeSpecifier.get());
        }
        else
        {
            m_TypeSpecifier = nullptr;
        }

        if (obj.m_Value)
        {
            m_Value = obj.m_Value->Clone();
        }
        else
        {
            m_Value = nullptr;
        }
    }

    Box<Statement> LetStatement::Parse(Parser &parser)
    {
        Span start = parser.Current().Span();
        Box<Identifier> identifier = Identifier::Parse(parser);
        parser.NextToken();
        Ref<TypeSpecifier> typeSpecifier = LetStatement::HandleTypeSpecifier(parser);
        if (parser.Current().TokenType() == TokenType::Semicolon)
        {
            if (typeSpecifier == nullptr)
            {
                ErrorSystem::AddError(parser, "Expect type specifier");
            }
            Span span{start.Start, parser.Current().Span().Start};
            return MakeBox<LetStatement>(std::move(identifier), typeSpecifier, nullptr, span);
        }
        parser.Expect(TokenType::Assign);
        parser.NextToken();
        Box<Expression> value = Expression::Parse(parser);
        parser.NextTokenAndExpect(TokenType::Semicolon);
        const Span &end = parser.Current().Span();
        return MakeBox<LetStatement>(std::move(identifier), typeSpecifier, std::move(value), Span(start.Start, end.End));
    }

    Ref<TypeSpecifier> LetStatement::HandleTypeSpecifier(Parser &parser)
    {
        if (parser.Current().TokenType() != TokenType::Colon)
        {
            return nullptr;
        }
        parser.NextToken();
        Ref<TypeSpecifier> typeSpecifier = TypeSpecifier::Parse(parser);
        parser.NextToken();
        return typeSpecifier;
    }
} // namespace Marble
