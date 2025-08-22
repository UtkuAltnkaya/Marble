#pragma once

#include "Ast/Ast.hpp"
#include "Ast/Definitions.hpp"
#include "Utils/Derive/Debug.hpp"

namespace Marble
{

    class Program : public Ast, public Derive::Debug
    {
    public:
        Program(std::vector<Box<Definition>> &&definitions, const Span &span);
        ~Program() = default;

        static Ref<Program> Parse(Parser &parser);
        inline const std::vector<Box<Definition>> &Definitions() const { return m_Definitions; }
        inline std::vector<Box<Definition>> &Definitions() { return m_Definitions; }

    private:
        std::vector<Box<Definition>> m_Definitions;

        DERIVE_DEBUG(Program, FIELD(m_Definitions))
    };

} // namespace Marble
