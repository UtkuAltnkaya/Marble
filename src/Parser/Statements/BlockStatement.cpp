
#include "Ast/Statements.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    static bool IsStatement(TokenType tokenType);

    BlockStatement::BlockStatement(std::vector<Box<Statement>> statements, const Span &span)
        : Statement{span, StatementType::Block}, m_Statements{std::move(statements)}
    {
    }

    Box<Statement> BlockStatement::Parse(Parser &parser)
    {
        Span start = parser.Current().Span();
        parser.NextTokenAndExpect(TokenType::OpenCurlyBrace);

        std::vector<Box<Statement>> statements;

        do
        {
            parser.NextToken();
            if (parser.Current().TokenType() == TokenType::CloseCurlyBrace)
            {
                break;
            }
            if (parser.Current().TokenType() == TokenType::Eof)
            {
                // throw SyntacticError(parser, "Missing close curly brace '}'");
                throw "Missing close curly brace '}'";
            }

            if (IsStatement(parser.Current().TokenType()))
            {
                statements.push_back(Statement::Parse(parser));
            }
            else if (parser.Current().TokenType() == TokenType::OpenCurlyBrace)
            {
                statements.push_back(BlockStatement::Parse(parser));
            }
            else
            {
                Box<Statement> expression = ExpressionStatement::Parse(parser);
                parser.NextTokenAndExpect(TokenType::Semicolon);
                statements.push_back(std::move(expression));
            }
        } while (true);

        const Span &end = parser.Current().Span();
        return MakeBox<BlockStatement>(std::move(statements), Span{start.Start, end.End});
    }

    bool IsStatement(TokenType tokenType)
    {
        return tokenType == TokenType::Let ||
               tokenType == TokenType::Return ||
               tokenType == TokenType::If ||
               tokenType == TokenType::For ||
               tokenType == TokenType::While ||
               tokenType == TokenType::Defer;
    }
}