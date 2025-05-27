#pragma once

#include "Utils/File.hpp"
#include "Marble/ArgParser.hpp"
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "SymbolTable/SymbolTable.hpp"

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
