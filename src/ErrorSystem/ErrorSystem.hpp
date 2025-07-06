#pragma once

#include <iostream>
#include <vector>
#include <string_view>

#include "Utils/Memory.hpp"

namespace Marble
{
    class Ast;
    class File;
    class Lexer;
    class Parser;
    class SemanticAnalyzer;
    class CompilerError;
    class CompilerWarning;

    class CompilationTerminatedException : public std::exception
    {
    public:
        const char *what() const noexcept override { return "Compilation terminated."; }
    };

    class ErrorSystem
    {
    public:
        static void Init();
        static void Shutdown();
        static ErrorSystem &GetInstance();

        static void AddError(std::string_view message, const File *const file = nullptr, bool shouldThrow = true);
        static void AddError(const Lexer &lexer, std::string_view message, bool shouldThrow = true);
        static void AddError(const Parser &parser, std::string_view message, bool shouldThrow = true);
        static void AddError(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::string_view message, bool shouldThrow = false);
        static void RemoveLastError();

        inline const std::vector<Box<CompilerError>> &GetErrors() const { return m_Errors; }
        inline const std::vector<Box<CompilerWarning>> &GetWarnings() const { return m_Warnings; }
        inline bool IsError() const { return m_Errors.size() != 0; }
        inline bool IsWarning() const { return m_Warnings.size() != 0; }

        void PrintError();
        void PrintWarning();
        friend std::ostream &operator<<(std::ostream &os, const ErrorSystem &error);

    private:
        ErrorSystem() = default;
        ~ErrorSystem() = default;
        ErrorSystem(const ErrorSystem &obj) = delete;
        ErrorSystem &operator=(const ErrorSystem &obj) = delete;

    private:
        std::vector<Box<CompilerError>> m_Errors;
        std::vector<Box<CompilerWarning>> m_Warnings;
    };

} // namespace Marble
