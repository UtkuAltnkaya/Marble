#include <iostream>
#include <fstream>
#include "Marble/Compiler.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    Compiler::Compiler(int argc, char const *argv[]) : m_ArgParserBuilder{argc, argv}
    {
        AddArgs();
        ErrorSystem::Init();
        SymbolTable::Init();
    }

    Compiler::~Compiler()
    {
        PrintAst();
        PrintSymbolTable();
        ErrorSystem::Shutdown();
        SymbolTable::ShutDown();
    }

    void Compiler::Compile()
    {
        ErrorSystem &errorSystem = ErrorSystem::GetInstance();
        ArgParser argParser = m_ArgParserBuilder.Build();
        argParser.Parse();

        if (argParser.GetValue<bool>("help").has_value())
        {
            std::cout << "Help" << std::endl;
            return;
        }

        auto optionalFileName = argParser.GetValue<std::string>("file");

        if (!optionalFileName)
        {
            ErrorSystem::AddError("Filename required");
        }
        File file{*optionalFileName};
        {
            Lexer lexer{file};
            Parser parser{lexer};
            m_Program = parser.Parse();
        }
        {
            SemanticAnalyzer semanticAnalyzer{m_Program, file};
            semanticAnalyzer.Analyze();
        }
        if (errorSystem.IsError())
        {
            throw CompilationTerminatedException();
        }

        if (false)
        {
            CodegenContext codegenContext{file.FileName()};
            codegenContext.Generate(m_Program);
            codegenContext.Print();
        }

        if (errorSystem.IsWarning())
        {
            errorSystem.PrintWarning();
        }
    }

    void Compiler::AddArgs()
    {
        m_ArgParserBuilder
            .AddValue("--file", "-f", "file")
            .AddValue("--output", "-o", "output")
            .AddFlag("--debug", "-d", "debug")
            .AddFlag("--object", "-c", "object")
            .AddFlag("--help", "-h", "help");
    }

    void Compiler::PrintAst()
    {
        std::ofstream file("out/ast.txt");
        file << *m_Program;
    }

    void Compiler::PrintSymbolTable()
    {

        std::ofstream file("out/symbolTable.txt");
        file << SymbolTable::Get();
    }

} // namespace Marble
