#include "Ast/Statements.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    IfStatement::IfStatement(Box<Expression> condition, Box<Statement> block, std::vector<Box<Statement>> &&elseIfStatements, Box<Statement> elseStatement, const Span &span)
        : Statement{span, StatementType::If}, m_Condition{std::move(condition)}, m_Block{std::move(block)},
          m_ElseIfStatements{std::move(elseIfStatements)}, m_ElseStatement{std::move(elseStatement)}
    {
    }

    IfStatement::IfStatement(const IfStatement &obj) : Statement{obj.m_Span, StatementType::If}
    {
        m_Condition = obj.m_Condition->Clone();
        m_Block = obj.m_Block->Clone();
        for (auto &elseIf : obj.m_ElseIfStatements)
        {
            m_ElseIfStatements.push_back(elseIf->Clone());
        }
        if (obj.m_ElseStatement)
        {
            m_ElseStatement = obj.m_ElseStatement->Clone();
        }
        else
        {
            m_ElseStatement = nullptr;
        }
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
            end = elseIfStatements.back()->GetSpan();
        }
        return MakeBox<IfStatement>(
            std::move(condition), std::move(block), std::move(elseIfStatements), std::move(elseStatement), Span{start.Start, end.Start});
    }

    ElseIfStatement::ElseIfStatement(Box<Expression> condition, Box<Statement> block, const Span &span)
        : Statement{span, StatementType::ElseIf}, m_Condition{std::move(condition)}, m_Block{std::move(block)}
    {
    }

    ElseIfStatement::ElseIfStatement(const ElseIfStatement &obj) : Statement{obj.m_Span, StatementType::ElseIf}
    {
        m_Condition = obj.m_Condition->Clone();
        m_Block = obj.m_Block->Clone();
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

    ElseStatement::ElseStatement(const ElseStatement &obj) : Statement{obj.m_Span, StatementType::Else}
    {
        m_Block = obj.m_Block->Clone();
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
