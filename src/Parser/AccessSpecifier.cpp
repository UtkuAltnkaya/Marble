#include "Ast/AccessSpecifier.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    AccessSpecifier ParseAccessSpecifier(Parser &parser, Span &span)
    {
        if (parser.Current().TokenType() != TokenType::Pub)
        {
            return AccessSpecifier::Private;
        }
        span = parser.Current().Span();
        parser.NextToken();
        return AccessSpecifier::Public;
    }
} // namespace Marble
