#pragma once

#include "Utils/File.hpp"
#include "Lexer/Token/Token.hpp"
#include "Lexer/Token/TokenType.hpp"

namespace MarbleTest
{
    using namespace Marble;

    class MockFile : public File
    {
    public:
        MockFile(const std::string &content)
        {
            m_Content = content;
            m_FileName = "main.mb";
        }
    };

    class MockToken : public Token
    {
    public:
        MockToken(Marble::TokenType tokenType) : Token{tokenType, "", Marble::Span{}}
        {
        }
    };

} // namespace MarbleTest
