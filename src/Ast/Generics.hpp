#pragma once

#include <vector>
#include "Ast/Ast.hpp"
#include "Ast/TypeSpecifier.hpp"

namespace Marble
{
    class Generics : public Ast
    {
    public:
        Generics(std::vector<Ref<TypeSpecifier>> &&types, const Span &span);
        ~Generics() = default;

        static Box<Generics> Parse(Parser &parser);
        inline const std::vector<Ref<TypeSpecifier>> &Types() const { return m_Types; }

        bool operator==(const Generics &obj) const
        {
            if (m_Types.size() != obj.m_Types.size())
            {
                return false;
            }
            for (size_t i = 0; i < m_Types.size(); i++)
            {
                if (m_Types[i]->operator!=(*obj.m_Types[i]))
                {
                    return false;
                }
            }
            return true;
        }

    private:
        std::vector<Ref<TypeSpecifier>> m_Types;
    };

} // namespace Marble
