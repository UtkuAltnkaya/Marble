#pragma once

#include <unordered_map>
#include "Ast/Program.hpp"
#include "Ast/Generics.hpp"

namespace Marble
{
    class GenericExpander
    {
    public:
        GenericExpander(Ref<Program> program);
        ~GenericExpander() = default;

        const std::string &Expand(const std::string &name, const std::vector<Ref<TypeSpecifier>> &typeArgs);

    private:
        Box<Identifier> GenerateConcreteName(const Identifier &id, const std::vector<Ref<TypeSpecifier>> &typeArgs);

    private:
        Ref<Program> m_Program;
        std::unordered_map<GenericInstanceKey, std::string, GenericInstanceKeyHasher> m_Generis;
    };

} // namespace Marble
