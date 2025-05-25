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

    private:
        std::vector<Ref<TypeSpecifier>> m_Types;
    };

} // namespace Marble
