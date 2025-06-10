#include "Ast/Statements.hpp"
#include "ErrorSystem/ErrorSystem.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{

    Statement::Statement(const Span &span, Marble::StatementType statementType) : Ast{span, AstType::Statement}, m_StatementType{statementType}
    {
    }

    Statement::Statement(Span &&span, Marble::StatementType statementType) : Ast{span, AstType::Statement}, m_StatementType{statementType}
    {
    }

    Box<Statement> Statement::Parse(Parser &parser)
    {
        switch (parser.Current().TokenType())
        {
        case TokenType::Let:
            return LetStatement::Parse(parser);
        case TokenType::Return:
            return ReturnStatement::Parse(parser);
        case TokenType::Defer:
            return DeferStatement::Parse(parser);
        case TokenType::For:
            return ForStatement::Parse(parser);
        case TokenType::While:
            return WhileStatement::Parse(parser);
        case TokenType::If:
            return IfStatement::Parse(parser);
        default:
            ErrorSystem::AddError(parser, "Unknown Statement!");
        }
        return nullptr;
    }
} // namespace Marble
