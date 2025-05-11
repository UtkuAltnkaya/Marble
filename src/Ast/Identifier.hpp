#pragma once

#include <string>
#include "Utils/Memory.hpp"
#include "Ast/Ast.hpp"

namespace Marble
{
    class Identifier : public Ast
    {
    public:
        Identifier(const std::string &id, const Span &span);
        Identifier(std::string &&id, const Span &span);

        Identifier(const Token &token);
        Identifier(Token &&token);

        Identifier(const std::string &id, Span &&span);
        Identifier(std::string &&id, Span &&span);

        Identifier(const Identifier &obj);
        Identifier(Identifier &&obj);

        ~Identifier() = default;

    public:
        static Box<Identifier> Parse(Parser &parser);

        inline const std::string &Id() const { return m_Id; }

    private:
        std::string m_Id;
    };

} // namespace Marble
