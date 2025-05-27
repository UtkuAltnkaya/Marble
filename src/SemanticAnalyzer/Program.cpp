#include "Ast/Program.hpp"

namespace Marble
{
    Ref<TypeSpecifier> Program::Analyze()
    {
        for (auto &definition : m_Definitions)
        {
            definition->Analyze();
        }
        return TypeSpecifierOk;
    }

} // namespace Marble
