#pragma once
#include <exception>

#include <string_view>
#include <string>
#include <sstream>

#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    class ErrorSystem;

    class CompilerError : public std::exception
    {
    public:
        virtual ~CompilerError() = default;
        CompilerError() = default;
        const char *what() const noexcept override { return m_Error.c_str(); }

    protected:
        std::string m_Error;
    };

    class GeneralError : public CompilerError
    {
    public:
        friend class ErrorSystem;
        ~GeneralError() = default;

    private:
        GeneralError(std::string_view message, const File *const file);
    };

    class LexicalError : public CompilerError
    {
    public:
        friend class ErrorSystem;
        ~LexicalError() = default;

    private:
        LexicalError(const Lexer &lexer, std::string_view message);
    };

    class ParserError : public CompilerError
    {
    public:
        friend class ErrorSystem;
        ~ParserError() = default;

    private:
        ParserError(const Parser &parser, std::string_view message);
    };

} // namespace Marble
