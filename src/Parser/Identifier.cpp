#include "Ast/Identifier.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{

    Identifier::Identifier(const std::string &id, const Span &span) : Ast{span, AstType::Identifier}, m_Id{id}
    {
    }

    Identifier::Identifier(std::string &&id, const Span &span) : Ast{span, AstType::Identifier}, m_Id{std::move(id)}
    {
    }

    Identifier::Identifier(const Token &token) : Ast{token.Span(), AstType::Identifier}, m_Id{token.Text()}
    {
    }

    Identifier::Identifier(Token &&token) : Ast{std::move(token.Span()), AstType::Identifier}, m_Id{std::move(token.Text())}
    {
    }

    Identifier::Identifier(const std::string &id, Span &&span) : Ast{std::move(span), AstType::Identifier}, m_Id{id}
    {
    }

    Identifier::Identifier(std::string &&id, Span &&span) : Ast{std::move(span), AstType::Identifier}, m_Id{std::move(id)}
    {
    }

    Identifier::Identifier(const Identifier &obj) : Ast{obj.m_Span, AstType::Identifier}, m_Id{obj.m_Id}
    {
    }

    Identifier::Identifier(Identifier &&obj) : Ast{std::move(obj.m_Span), AstType::Identifier}, m_Id{std::move(obj.m_Id)}
    {
    }

    Identifier &Identifier::operator=(const Identifier &obj)
    {
        if (this == &obj)
        {
            return *this;
        }
        m_Span = obj.m_Span;
        m_Type = obj.m_Type;
        m_Id = obj.m_Id;
        return *this;
    }

    Box<Identifier> Identifier::Parse(Parser &parser)
    {
        const Token &token = parser.NextTokenAndExpect(TokenType::Identifier);
        return MakeBox<Identifier>(token);
    }

} // namespace Marble