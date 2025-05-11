#include "Compiler.hpp"

namespace Marble
{
    Compiler::Compiler(int argc, char const *argv[]) : m_File{"main.mb"}, m_Lexer{m_File}
    {
    }

    Compiler::~Compiler()
    {
    }

    void Compiler::Compile()
    {
    }

} // namespace Marble
