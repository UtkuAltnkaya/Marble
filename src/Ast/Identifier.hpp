#pragma once

#include <string>
#include "Utils/Memory.hpp"
#include "Ast/Ast.hpp"
#include "Utils/Derive/Debug.hpp"

namespace Marble
{
    class Identifier : public Ast, public Derive::Debug
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

        Identifier &operator=(const Identifier &obj);

        static Box<Identifier> Parse(Parser &parser);
        inline const std::string &Id() const { return m_Id; }
        void Id(const std::string &id);

        inline bool operator==(const Identifier &obj) const { return m_Id == obj.m_Id; }
        inline bool operator!=(const Identifier &obj) const { return m_Id != obj.m_Id; }
        inline const std::string &operator*() const { return m_Id; }
        inline std::string &operator*() { return m_Id; }

    private:
        std::string m_Id;
        DERIVE_DEBUG(Identifier, FIELD(m_Id))
    };

} // namespace Marble
