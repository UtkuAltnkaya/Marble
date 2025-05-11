#include "Ast/Expressions.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    Box<Expression> Expression::Parse(Parser &parser, Precedence precedence)
    {
        switch (precedence)
        {
        case Precedence::Binary:
            return BinaryExpression::Parse(parser, precedence);
        case Precedence::Unary:
            return UnaryExpression::Parse(parser, precedence);
        case Precedence::Assignment:
            return AssignmentExpression::Parse(parser, precedence);
        case Precedence::ArrayIndex:
            return ArrayIndexExpression::Parse(parser, precedence);
        case Precedence::MemberAccess:
            return MemberAccessExpression::Parse(parser, precedence);
        case Precedence::FunctionCall:
            return FunctionCallExpression::Parse(parser, precedence);
        case Precedence::ArrayInit:
            return ArrayInitExpression::Parse(parser, precedence);
        case Precedence::ObjectInit:
            return ObjectInitExpression::Parse(parser, precedence);
        case Precedence::Cast:
            return CastExpression::Parse(parser, precedence);
        case Precedence::Identifier:
            return IdentifierExpression::Parse(parser, precedence);
        case Precedence::Primitive:
            return PrimitiveExpression::Parse(parser, precedence);
        case Precedence::NameSpace:
            return NamespaceExpression::Parse(parser, precedence);
        case Precedence::END:
            throw "Unknown Expression";
            // throw SyntacticError(parser, "Unknown Expression");
        default:
            return nullptr;
        }
    }

    Precedence Expression::NextPrecedence(Precedence precedence)
    {
        if ((int)precedence < (int)Precedence::END - (int)Precedence::START - 1)
        {
            return (Precedence)((int)precedence + 1);
        }
        return Precedence::END;
    }

} // namespace Marble
