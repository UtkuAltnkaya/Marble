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
        virtual Box<Statement> Clone() = 0;
        virtual void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) = 0;

    protected:
        Marble::StatementType m_StatementType;
    };

    class LetStatement : public Statement
    {
    public:
        LetStatement(Box<Identifier> identifier, Ref<TypeSpecifier> typeSpecifier, Box<Expression> value, const Span &span);
        LetStatement(const LetStatement &obj);
        ~LetStatement() = default;

        static Box<Statement> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;

        inline const Identifier &GetIdentifier() const { return *m_Identifier.get(); }
        inline Ref<TypeSpecifier> GetTypeSpecifier() const { return m_TypeSpecifier; }
        inline const Expression &GetValue() const { return *m_Value.get(); }
        virtual Box<Statement> Clone() override;
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

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
        ReturnStatement(const ReturnStatement &obj);
        ~ReturnStatement() = default;

        static Box<Statement> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        inline const Expression *const GetExpression() const { return m_Expression.get(); }
        virtual Box<Statement> Clone() override;
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

    private:
        Box<Expression> m_Expression;
    };

    class DeferStatement : public Statement
    {
    public:
        DeferStatement(Box<Expression> expression, const Span &span);
        DeferStatement(const DeferStatement &obj);
        ~DeferStatement() = default;

        static Box<Statement> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        inline const Expression &GetExpression() const { return *m_Expression.get(); }
        virtual Box<Statement> Clone() override;
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

    private:
        Box<Expression> m_Expression;
    };

    class BlockStatement : public Statement
    {
    public:
        BlockStatement(std::vector<Box<Statement>> statements, const Span &span);
        BlockStatement(const BlockStatement &obj);
        ~BlockStatement() = default;

        static Box<Statement> Parse(Parser &parser, bool passTokenCheck = false);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        virtual Box<Statement> Clone() override;

        inline const std::vector<Box<Statement>> &Statements() const { return m_Statements; }
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

    private:
        std::vector<Box<Statement>> m_Statements;
    };

    class ForStatement : public Statement
    {
    public:
        ForStatement(Box<Statement> letStatement, Box<Expression> condition, Box<Expression> increment, Box<Statement> block, const Span &span);
        ForStatement(Box<Expression> assignmentExpression, Box<Expression> condition, Box<Expression> increment, Box<Statement> block, const Span &span);
        ForStatement(const ForStatement &obj);
        ~ForStatement() = default;

        static Box<Statement> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        virtual Box<Statement> Clone() override;
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

        inline const Statement *const GetLetStatement() const { return m_LetStatement.get(); }
        inline const Expression *const GetAssignmentExpression() const { return m_AssignmentExpression.get(); }
        inline const Expression &GetCondition() const { return *m_Condition.get(); }
        inline const Expression &GetIncrement() const { return *m_Increment.get(); }
        inline const Statement &GetBlock() const { return *m_Block.get(); }

    private:
        Box<Statement> m_LetStatement;
        Box<Expression> m_AssignmentExpression;
        Box<Expression> m_Condition;
        Box<Expression> m_Increment;
        Box<Statement> m_Block;
    };

    class WhileStatement : public Statement
    {
    public:
        WhileStatement(Box<Expression> condition, Box<Statement> block, const Span &span);
        WhileStatement(const WhileStatement &obj);
        ~WhileStatement() = default;

        static Box<Statement> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        virtual Box<Statement> Clone() override;
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

        inline const Expression &GetCondition() const { return *m_Condition.get(); }
        inline const Statement &GetBlock() const { return *m_Block.get(); }

    private:
        Box<Expression> m_Condition;
        Box<Statement> m_Block;
    };

    class ElseStatement : public Statement
    {
    public:
        ElseStatement(Box<Statement> block, const Span &span);
        ElseStatement(const ElseStatement &obj);
        ~ElseStatement() = default;

        static Box<Statement> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        inline const Statement &GetBlock() const { return *m_Block.get(); };
        virtual Box<Statement> Clone() override;
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

    private:
        Box<Statement> m_Block;
    };

    class ElseIfStatement : public Statement
    {
    public:
        ElseIfStatement(Box<Expression> condition, Box<Statement> block, const Span &span);
        ElseIfStatement(const ElseIfStatement &obj);
        ~ElseIfStatement() = default;

        static void Parse(Parser &parser, std::vector<Box<Statement>> &elseIfStatements);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        virtual Box<Statement> Clone() override;
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

        inline const Expression &GetCondition() const { return *m_Condition.get(); };
        inline const Statement &GetBlock() const { return *m_Block.get(); };

    private:
        Box<Expression> m_Condition;
        Box<Statement> m_Block;
    };

    class IfStatement : public Statement
    {

    public:
        IfStatement(Box<Expression> condition, Box<Statement> block, std::vector<Box<Statement>> &&elseIfStatements, Box<Statement> elseStatement, const Span &span);
        IfStatement(const IfStatement &obj);
        ~IfStatement() = default;

        static Box<Statement> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        virtual Box<Statement> Clone() override;
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

        inline const Expression &GetCondition() const { return *m_Condition.get(); };
        inline const Statement &GetBlock() const { return *m_Block.get(); }
        inline const std::vector<Box<Statement>> &GetElseIfStatements() const { return m_ElseIfStatements; }
        inline const Statement *const GetElseStatement() const { return m_ElseStatement.get(); }

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
        ExpressionStatement(const ExpressionStatement &obj)
            : Statement{obj.m_Span, StatementType::Expression}, m_Expression{obj.m_Expression->Clone()} {}
        ~ExpressionStatement() = default;

        static Box<Statement> Parse(Parser &parser)
        {
            return MakeBox<ExpressionStatement>(Expression::Parse(parser));
        }

        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override
        {
            m_Expression->Analyze(semanticAnalyzer);
            return TypeSpecifierOk;
        }

        virtual Box<Statement> Clone() override
        {
            return MakeBox<ExpressionStatement>(*this);
        }

        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override
        {
            m_Expression->SubstituteGenerics(map);
        }

        inline const Expression &GetExpression() const { return *m_Expression.get(); }

    private:
        Box<Expression> m_Expression;
    };

} // namespace Marble
