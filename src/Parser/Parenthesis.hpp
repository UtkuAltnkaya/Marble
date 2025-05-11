#pragma once

#include <functional>
#include "Parser/Parser.hpp"
#include <vector>

namespace Marble
{
    namespace Parenthesis
    {
        template <class T>
        using CallbackType = std::function<T(Parser &parser)>;

        template <class T>
        void Parse(std::vector<T> &vec, Parser &parser, TokenType closeToken, CallbackType<T> callback)
        {
            while (parser.Current().TokenType() != closeToken)
            {
                parser.NextToken();
                if (parser.Current().TokenType() == closeToken)
                {
                    break;
                }
                if (parser.Current().TokenType() == TokenType::Eof)
                {
                    throw std::string("Missing") + TokenTypeToString(closeToken);
                    // throw SyntacticError(parser, std::string("Missing") + token_type_to_str(close));
                }
                vec.push_back(callback(parser));

                parser.NextToken();

                if (parser.Current().TokenType() == TokenType::Comma)
                {
                    continue;
                }
                if (parser.Current().TokenType() == closeToken)
                {
                    break;
                }
                throw std::string("Expect Comma or ") + TokenTypeToString(closeToken) + " but found " + TokenTypeToString(parser.Current().TokenType());
                // throw SyntacticError(parser, std::string("Expect Comma or ") + token_type_to_str(close) + " but found " + token_type_to_str(parser.get_current().get_token_type()));
            }
        }

    } // namespace Parenthesis
} // namespace Marble
