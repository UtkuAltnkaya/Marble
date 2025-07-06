#include <assert.h>

#include "ErrorSystem/ErrorSystem.hpp"
#include "ErrorSystem/CompilerError.hpp"
#include "ErrorSystem/CompilerWarning.hpp"
#include "ErrorSystem.hpp"

namespace Marble
{
    static ErrorSystem *s_Instance = nullptr;

    void ErrorSystem::Init()
    {
        ASSERT_A(!s_Instance, "Singleton instance is already initalized");
        s_Instance = new ErrorSystem;
    }

    void ErrorSystem::Shutdown()
    {
        ASSERT_A(s_Instance, "Singleton instance is not initalized");
        delete s_Instance;
    }

    ErrorSystem &ErrorSystem::GetInstance()
    {
        ASSERT_A(s_Instance, "Singleton instance is not initalized");
        return *s_Instance;
    }

    void ErrorSystem::AddError(std::string_view message, const File *const file, bool shouldThrow)
    {
        ErrorSystem &errorSystem = ErrorSystem::GetInstance();
        GeneralError *error = new GeneralError{message, file};
        errorSystem.m_Errors.emplace_back(Box<GeneralError>(error));
        if (shouldThrow)
        {
            throw CompilationTerminatedException();
        }
    }

    void ErrorSystem::AddError(const Lexer &lexer, std::string_view message, bool shouldThrow)
    {
        ErrorSystem &errorSystem = ErrorSystem::GetInstance();
        LexicalError *error = new LexicalError{lexer, message};
        errorSystem.m_Errors.emplace_back(Box<LexicalError>(error));
        if (shouldThrow)
        {
            throw CompilationTerminatedException();
        }
    }

    void ErrorSystem::AddError(const Parser &parser, std::string_view message, bool shouldThrow)
    {
        ErrorSystem &errorSystem = ErrorSystem::GetInstance();
        ParserError *error = new ParserError{parser, message};
        errorSystem.m_Errors.emplace_back(Box<ParserError>(error));
        if (shouldThrow)
        {
            throw CompilationTerminatedException();
        }
    }

    void ErrorSystem::AddError(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::string_view message, bool shouldThrow)
    {
        ErrorSystem &errorSystem = ErrorSystem::GetInstance();
        SemanticError *error = new SemanticError{semanticAnalyzer, node, message};
        errorSystem.m_Errors.emplace_back(Box<SemanticError>(error));
        if (shouldThrow)
        {
            throw CompilationTerminatedException();
        }
    }

    void ErrorSystem::RemoveLastError()
    {
        ErrorSystem &errorSystem = ErrorSystem::GetInstance();
        errorSystem.m_Errors.pop_back();
    }

    void ErrorSystem::PrintError()
    {
        for (auto &errors : m_Errors)
        {
            std::cerr << errors->what() << "\n";
        }
    }

    void ErrorSystem::PrintWarning()
    {
        for (auto &warning : m_Warnings)
        {
            std::cerr << warning->warn() << "\n";
        }
    }

    std::ostream &operator<<(std::ostream &os, const ErrorSystem &errorSystem)
    {
        if (errorSystem.IsWarning())
        {
            os << "Warnings:\n";
        }
        for (auto &warning : errorSystem.m_Warnings)
        {
            os << warning->warn() << "\n";
        }
        if (errorSystem.IsError())
        {
            os << "Errors:\n";
        }
        for (auto &error : errorSystem.m_Errors)
        {
            os << error->what() << "\n";
        }
        return os;
    }
} // namespace Marble
