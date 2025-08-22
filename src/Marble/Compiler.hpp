#pragma once

#include "Utils/File.hpp"
#include "Marble/ArgParser.hpp"
#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "Codegen/Codegen.hpp"

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
        void PrintAst();
        void PrintSymbolTable();

    private:
        ArgParserBuilder m_ArgParserBuilder;
        Ref<Program> m_Program;
    };

} // namespace Marble
