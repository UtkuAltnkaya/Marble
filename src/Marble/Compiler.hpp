#pragma once

#include "Utils/File.hpp"
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "Marble/ArgParser.hpp"

namespace Marble
{
    class Compiler
    {
    public:
        Compiler(int argc, char const *argv[]);
        ~Compiler();

        void Compile();

    private:
        void AddArgs();

    private:
        ArgParserBuilder m_ArgParserBuilder;
    };

} // namespace Marble
