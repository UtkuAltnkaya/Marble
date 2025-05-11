#pragma once

#include "Utils/Memory.hpp"
#include "Lexer/Lexer.hpp"
#include "Lexer/Token/Token.hpp"

namespace Marble
{
    namespace TokenFinder
    {
        Box<Token> Eof(Lexer &lexer);
        Box<Token> IdOrKeyword(Lexer &lexer);
        Box<Token> Number(Lexer &lexer);
        Box<Token> String(Lexer &lexer);
        Box<Token> Char(Lexer &lexer);
        Box<Token> Deliminator(Lexer &lexer);
        Box<Token> Operator(Lexer &lexer);
    } // namespace TokenFinder

} // namespace Marble
