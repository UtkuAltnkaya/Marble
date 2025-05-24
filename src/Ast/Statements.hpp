#pragma once

#include "Ast/Ast.hpp"
#include "Ast/Identifier.hpp"
#include "Ast/TypeSpecifier.hpp"
#include "Ast/Expressions.hpp"

namespace Marble
{
    enum class StatementType
    {
        Let,
        Return,
        Defer,
        For,
        While,
        If,
        ElseIf,
        Else,
        Block,
        Expression,
    };

    class Statement : public Ast
    {
    public:
        Statement(const Span &span, StatementType statementType);
        Statement(Span &&span, StatementType statementType);
        virtual ~Statement() = default;

        static Box<Statement> Parse(Parser &parser);
        inline Marble::StatementType StatementType() const { return m_StatementType; }

    protected:
        Marble::StatementType m_StatementType;
    };

    class LetStatement : public Statement
    {
    public:
        LetStatement(Box<Identifier> identifier, Ref<TypeSpecifier> typeSpecifier, Box<Expression> value, const Span &span);
        ~LetStatement() = default;

        static Box<Statement> Parse(Parser &parser);

    private:
        static Ref<TypeSpecifier> HandleTypeSpecifier(Parser &parser);

    private:
        Box<Identifier> m_Identifier;
        Ref<TypeSpecifier> m_TypeSpecifier;
        Box<Expression> m_Value;
    };

    class ReturnStatement : public Statement
    {
    public:
        ReturnStatement(Box<Expression> expression, const Span &span);
        ~ReturnStatement() = default;

        static Box<Statement> Parse(Parser &parser);

    private:
        Box<Expression> m_Expression;
    };

    class DeferStatement : public Statement
    {
    public:
        DeferStatement(Box<Expression> expression, const Span &span);
        ~DeferStatement() = default;

        static Box<Statement> Parse(Parser &parser);

    private:
        Box<Expression> m_Expression;
    };

    class BlockStatement : public Statement
    {
    public:
        BlockStatement(std::vector<Box<Statement>> statements, const Span &span);
        ~BlockStatement() = default;

        static Box<Statement> Parse(Parser &parser);

    private:
        std::vector<Box<Statement>> m_Statements;
    };

    class ForStatement : public Statement
    {
    public:
        ForStatement(Box<Statement> letStatement, Box<Expression> condition, Box<Expression> increment, Box<Statement> block, const Span &span);
        ForStatement(Box<Expression> assignmentExpression, Box<Expression> condition, Box<Expression> increment, Box<Statement> block, const Span &span);
        ~ForStatement() = default;

        static Box<Statement> Parse(Parser &parser);

    private:
        Box<Statement> m_LetStatement;
        Box<Expression> m_AssignmentExpression;
        Box<Expression> m_Condition;
        Box<Expression> m_increment;
        Box<Statement> m_Block;
    };

    class WhileStatement : public Statement
    {
    public:
        WhileStatement(Box<Expression> condition, Box<Statement> block, const Span &span);
        ~WhileStatement() = default;
        static Box<Statement> Parse(Parser &parser);

    private:
        Box<Expression> m_Condition;
        Box<Statement> m_Block;
    };

    class ElseStatement : public Statement
    {
    public:
        ElseStatement(Box<Statement> block, const Span &span);
        ~ElseStatement() = default;
        static Box<Statement> Parse(Parser &parser);

    private:
        Box<Statement> m_Block;
    };

    class ElseIfStatement : public Statement
    {
    public:
        ElseIfStatement(Box<Expression> condition, Box<Statement> block, const Span &span);
        ~ElseIfStatement() = default;
        static void Parse(Parser &parser, std::vector<Box<Statement>> &elseIfStatements);

    private:
        Box<Expression> m_Condition;
        Box<Statement> m_Block;
    };

    class IfStatement : public Statement
    {

    public:
        IfStatement(Box<Expression> condition, Box<Statement> block, std::vector<Box<Statement>> &&elseIfStatements, Box<Statement> elseStatement, const Span &span);
        ~IfStatement() = default;

        static Box<Statement> Parse(Parser &parser);

    private:
        Box<Expression> m_Condition;
        Box<Statement> m_Block;
        std::vector<Box<Statement>> m_ElseIfStatements;
        Box<Statement> m_ElseStatement;
    };

    class ExpressionStatement : public Statement
    {
    public:
        ExpressionStatement(Box<Expression> expression)
            : Statement{expression->GetSpan(), StatementType::Expression}, m_Expression{std::move(expression)} {}
        ~ExpressionStatement() = default;

        static Box<Statement> Parse(Parser &parser)
        {
            return MakeBox<ExpressionStatement>(Expression::Parse(parser));
        }

    private:
        Box<Expression> m_Expression;
    };

} // namespace Marble
