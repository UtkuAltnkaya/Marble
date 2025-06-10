#pragma once

#include <functional>
#include <vector>

#include "Parser/Parser.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

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
                    ErrorSystem::AddError(parser, std::string("Missing") + TokenTypeToString(closeToken));
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
                std::string errMessage = std::string("Expect Comma or ") + TokenTypeToString(closeToken) + " but found " + TokenTypeToString(parser.Current().TokenType());
                ErrorSystem::AddError(parser, errMessage);
            }
        }

    } // namespace Parenthesis
} // namespace Marble
