#include "Ast/Statements.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    IfStatement::IfStatement(Box<Expression> condition, Box<Statement> block, std::vector<Box<Statement>> &&elseIfStatements, Box<Statement> elseStatement, const Span &span)
        : Statement{span, StatementType::If}, m_Condition{std::move(condition)},
          m_ElseIfStatements{std::move(elseIfStatements)}, m_ElseStatement{std::move(elseStatement)}
    {
    }

    Box<Statement> IfStatement::Parse(Parser &parser)
    {
        Span start = parser.Current().Span();
        parser.NextTokenAndExpect(TokenType::OpenParen);
        parser.NextToken();
        Box<Expression> condition = Expression::Parse(parser);
        parser.NextTokenAndExpect(TokenType::CloseParen);
        Box<Statement> block = BlockStatement::Parse(parser);
        Span end = parser.Current().Span();

        std::vector<Box<Statement>> elseIfStatements;
        ElseIfStatement::Parse(parser, elseIfStatements);

        Box<Statement> elseStatement = ElseStatement::Parse(parser);

        if (elseStatement)
        {
            end = elseStatement->GetSpan();
        }
        else if (elseIfStatements.size() > 0)
        {
            end = elseIfStatements.end()->get()->GetSpan();
        }
        return MakeBox<IfStatement>(
            std::move(condition), std::move(block), std::move(elseIfStatements), std::move(elseStatement), Span{start.Start, end.Start});
    }

    ElseIfStatement::ElseIfStatement(Box<Expression> condition, Box<Statement> block, const Span &span)
        : Statement{span, StatementType::ElseIf}, m_Condition{std::move(condition)}, m_Block{std::move(block)}
    {
    }

    void ElseIfStatement::Parse(Parser &parser, std::vector<Box<Statement>> &elseIfStatements)
    {
        if (parser.Next().TokenType() != TokenType::Else)
        {
            return;
        }
        Span start = parser.Current().Span();

        parser.NextToken();
        if (parser.Next().TokenType() != TokenType::If)
        {
            return;
        }

        parser.NextToken();

        parser.NextTokenAndExpect(TokenType::OpenParen);
        parser.NextToken();
        Box<Expression> condition = Expression::Parse(parser);
        parser.NextTokenAndExpect(TokenType::CloseParen);
        Box<Statement> block = BlockStatement::Parse(parser);

        Span span{start.Start, block->GetSpan().Start};

        Box<Statement> elseIfStatement = MakeBox<ElseIfStatement>(std::move(condition), std::move(block), span);
        elseIfStatements.push_back(std::move(elseIfStatement));

        return ElseIfStatement::Parse(parser, elseIfStatements);
    }

    ElseStatement::ElseStatement(Box<Statement> block, const Span &span)
        : Statement{span, StatementType::Else}, m_Block{std::move(block)}
    {
    }

    Box<Statement> ElseStatement::Parse(Parser &parser)
    {
        if (parser.Current().TokenType() != TokenType::Else)
        {
            return nullptr;
        }
        Span start = parser.Current().Span();
        Box<Statement> block = BlockStatement::Parse(parser);
        const Span &end = parser.Current().Span();
        return MakeBox<ElseStatement>(std::move(block), Span{start.Start, end.End});
    }

} // namespace Marble
