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

    private:
        std::vector<Box<Definition>> m_Definitions;
    };

} // namespace Marble
