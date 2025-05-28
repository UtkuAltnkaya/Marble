#pragma once

#include "Ast/Ast.hpp"
#include "Ast/Definitions.hpp"

namespace Marble
{

    class Program : public Ast
    {
    public:
        Program(std::vector<Box<Definition>> &&definitions, const Span &span);
        ~Program() = default;

        static Ref<Program> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze() override;
        inline const std::vector<Box<Definition>> &Definitions() const { return m_Definitions; }

    private:
        std::vector<Box<Definition>> m_Definitions;
    };

} // namespace Marble
