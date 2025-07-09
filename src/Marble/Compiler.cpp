#include <iostream>
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
        ErrorSystem::Shutdown();
        SymbolTable::ShutDown();
    }

    void Compiler::Compile()
    {
        ArgParser argParser = m_ArgParserBuilder.Build();
        argParser.Parse();

        if (argParser.GetValue<bool>("help").has_value())
        {
            std::cout << "Help" << std::endl;
            return;
        }

        auto optionalFileName = argParser.GetValue<std::string>("file");

        if (!optionalFileName.has_value())
        {
            ErrorSystem::AddError("Filename required");
        }
        File file{optionalFileName.value()};
        Ref<Program> program = nullptr;
        {
            Lexer lexer{file};
            Parser parser{lexer};
            program = parser.Parse();
        }
        {
            SemanticAnalyzer semanticAnalyzer{program, file};
            semanticAnalyzer.Analyze();
        }
        if (ErrorSystem::GetInstance().IsError())
        {
            throw CompilationTerminatedException();
        }
        // CodegenContext codegenContext{file.FileName()};
        // codegenContext.Generate(program);
        // codegenContext.Print();
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

} // namespace Marble
