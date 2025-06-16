#include "TokenType.hpp"
#include "Utils/Macros.hpp"

namespace Marble
{
    const char *TokenTypeToString(TokenType tokenType)
    {
        switch (tokenType)
        {
        case TokenType::Assign:
            return "Assign";
        case TokenType::Plus:
            return "Plus";
        case TokenType::Minus:
            return "Minus";
        case TokenType::Multiply:
            return "Multiply";
        case TokenType::Divide:
            return "Divide";
        case TokenType::Percent:
            return "Percent";
        case TokenType::Equal:
            return "Equal";
        case TokenType::NotEqual:
            return "NotEqual";
        case TokenType::LessThan:
            return "LessThan";
        case TokenType::GreaterThan:
            return "GreaterThan";
        case TokenType::LessEqual:
            return "LessEqual";
        case TokenType::GreaterEqual:
            return "GreaterEqual";
        case TokenType::BitAnd:
            return "BitAnd";
        case TokenType::BitOr:
            return "BitOr";
        case TokenType::BitXor:
            return "BitXor";
        case TokenType::BitLeft:
            return "BitLeft";
        case TokenType::BitRight:
            return "BitRight";
        case TokenType::And:
            return "And";
        case TokenType::Or:
            return "Or";
        case TokenType::Bang:
            return "Bang";
        case TokenType::Tilde:
            return "Tilde";
        case TokenType::Increment:
            return "Increment";
        case TokenType::Decrement:
            return "Decrement";
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::OpenParen:
            return "OpenParen";
        case TokenType::CloseParen:
            return "CloseParen";
        case TokenType::OpenCurlyBrace:
            return "OpenCurlyBrace";
        case TokenType::CloseCurlyBrace:
            return "CloseCurlyBrace";
        case TokenType::OpenBracket:
            return "OpenBracket";
        case TokenType::CloseBracket:
            return "CloseBracket";
        case TokenType::Let:
            return "Let";
        case TokenType::Fn:
            return "Fn";
        case TokenType::Break:
            return "Break";
        case TokenType::Case:
            return "Case";
        case TokenType::CharKeyword:
            return "CharKeyword";
        case TokenType::Const:
            return "Const";
        case TokenType::Continue:
            return "Continue";
        case TokenType::Default:
            return "Default";
        case TokenType::Do:
            return "Do";
        case TokenType::Double:
            return "Double";
        case TokenType::Else:
            return "Else";
        case TokenType::Enum:
            return "Enum";
        case TokenType::Float:
            return "Float";
        case TokenType::For:
            return "For";
        case TokenType::If:
            return "If";
        case TokenType::Int:
            return "Int";
        case TokenType::Bool:
            return "Bool";
        case TokenType::Return:
            return "Return";
        case TokenType::Usize:
            return "Usize";
        case TokenType::Sizeof:
            return "Sizeof";
        case TokenType::Static:
            return "Static";
        case TokenType::Struct:
            return "Struct";
        case TokenType::Switch:
            return "Switch";
        case TokenType::Void:
            return "Void";
        case TokenType::While:
            return "While";
        case TokenType::Str:
            return "Str";
        case TokenType::Impl:
            return "Impl";
        case TokenType::Pub:
            return "Pub";
        case TokenType::True:
            return "True";
        case TokenType::False:
            return "False";
        case TokenType::Defer:
            return "Defer";
        case TokenType::As:
            return "As";
        case TokenType::Number:
            return "Number";
        case TokenType::String:
            return "String";
        case TokenType::Char:
            return "Char";
        case TokenType::Semicolon:
            return "Semicolon";
        case TokenType::Colon:
            return "Colon";
        case TokenType::Comma:
            return "Comma";
        case TokenType::Dot:
            return "Dot";
        case TokenType::Arrow:
            return "Arrow";
        case TokenType::Null:
            return "Null";
        case TokenType::Eof:
            return "Eof";
        default:
            UNREACHABLE();
        }
    }

    std::ostream &operator<<(std::ostream &os, TokenType tokenType)
    {
        return os << TokenTypeToString(tokenType);
    }

} // namespace Marble
