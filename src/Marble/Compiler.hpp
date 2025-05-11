#pragma once

#include "File.hpp"
#include "Lexer/Lexer.hpp"

namespace Marble
{
    class Compiler
    {
    public:
        Compiler(int argc, char const *argv[]);
        ~Compiler();

        void Compile();

    private:
        File m_File;
        Lexer m_Lexer;
    };

} // namespace Marble
