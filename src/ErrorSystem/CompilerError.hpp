#pragma once
#include <exception>

#include <string_view>
#include <string>
#include <sstream>

#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerWarning.hpp"
#include "ErrorSystem/MessageHolder.hpp"

namespace Marble
{
    class ErrorSystem;
    class Ast;

    class CompilerError : public std::exception, public virtual MessageHolder
    {
    public:
        virtual ~CompilerError() = default;
        CompilerError() = default;
        const char *what() const noexcept override { return m_Message.c_str(); }
    };

    class GeneralError : public CompilerError, public CompilerWarning
    {
    public:
        friend class ErrorSystem;
        ~GeneralError() = default;

    private:
        GeneralError(std::string_view message, const File *const file);
    };

    class LexicalError : public CompilerError, public CompilerWarning
    {
    public:
        friend class ErrorSystem;
        ~LexicalError() = default;

    private:
        LexicalError(const Lexer &lexer, std::string_view message);
    };

    class ParserError : public CompilerError, public CompilerWarning
    {
    public:
        friend class ErrorSystem;
        ~ParserError() = default;

    private:
        ParserError(const Parser &parser, std::string_view message);
    };

    class SemanticError : public CompilerError, public CompilerWarning
    {
    public:
        friend class ErrorSystem;
        ~SemanticError() = default;

    private:
        SemanticError(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::string_view message);
    };

} // namespace Marble
