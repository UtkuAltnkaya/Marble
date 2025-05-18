#pragma once

#include "Ast/Ast.hpp"

namespace Marble
{

    enum class AccessSpecifier
    {
        Public,
        Private,
    };

    AccessSpecifier ParseAccessSpecifier(Parser &parser, Span &span);

} // namespace Marble
