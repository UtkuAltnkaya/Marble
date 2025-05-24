#include "Compiler.hpp"
#include <iostream>

namespace Marble
{
    Compiler::Compiler(int argc, char const *argv[]) : m_ArgParserBuilder{argc, argv}
    {
        AddArgs();
    }

    Compiler::~Compiler()
    {
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
            throw "Filename required";
        }

        File file{optionalFileName.value()};
        Lexer lexer{file};
        Parser parser{lexer};

        Ref<Program> program = parser.Parse();
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
