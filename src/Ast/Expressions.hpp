#pragma once

#include "Ast/Ast.hpp"
#include "Ast/TypeSpecifier.hpp"
#include <optional>
#include <functional>

namespace Marble
{
    enum class ExpressionType
    {
        ArrayInit,
        ObjectInit,
        ObjectField,
        Binary,
        Unary,
        Assignment,
        MemberAccess,
        ArrayIndex,
        FunctionCall,
        Cast,
        NameSpace,
        Identifier,
        Primitive,
    };

    enum class Precedence
    {
        START,
        Binary,
        Unary,
        Cast,
        Primitive,
        Assignment,
        MemberAccess,
        FunctionCall,
        ArrayIndex,
        NameSpace,
        ArrayInit,
        ObjectInit,
        Identifier,
        END,
    };
    enum class BinaryOperators
    {
        Add,
        Subtract,
        Multiply,
        Divide,
        Modulo,
        Equal,
        NotEqual,
        And,
        Or,
        BitAnd,
        BitOr,
        BitXor,
        GreaterThan,
        LessThan,
        GreaterThanOrEqual,
        LessThanOrEqual,
        BitLeft,
        BitRight,
    };

    enum class BinaryPrecedence
    {
        START,
        Or,
        And,
        BitOr,
        BitXor,
        BitAnd,
        BitLeft,
        BitRight,
        Equal,
        Relational,
        Sum,
        Product,
        END,
    };

    enum class UnaryOperators
    {
        Plus,
        Minus,
        Increment,
        Decrement,
        LogicalNot,
        BitwiseNot,
        Address,
        Indirection,
    };

    enum class UnaryExpressionType
    {
        Prefix,
        PostFix,
    };

    class Expression : public Ast
    {
    public:
        Expression(const Span &span, ExpressionType expressionType)
            : Ast{span, AstType::Expression}, m_ExpressionType{expressionType} {};

        Expression(Span &&span, ExpressionType expressionType)
            : Ast{std::move(span), AstType::Expression}, m_ExpressionType{expressionType} {};

        virtual ~Expression() = default;

    public:
        static Box<Expression> Parse(Parser &parser, Precedence precedence = DefaultPrecedence());
        static Precedence NextPrecedence(Precedence precedence);
        inline Marble::ExpressionType ExpressionType() const { return m_ExpressionType; }
        constexpr inline static Precedence DefaultPrecedence() { return (Precedence)((int)(Precedence::START) + 1); }

    protected:
        Marble::ExpressionType m_ExpressionType;
    };

    class BinaryExpression : public Expression
    {
    public:
        BinaryExpression(Box<Expression> left, BinaryOperators binaryOperator, Box<Expression> right, const Span &span);
        BinaryExpression(Box<Expression> left, BinaryOperators binaryOperator, Box<Expression> right, Span &&span);
        ~BinaryExpression() = default;

        inline const Expression &Left() const { return *m_Left.get(); }
        inline BinaryOperators Operator() const { return m_Operator; }
        inline const Expression &Right() const { return *m_Left.get(); }

    public:
        static Box<Expression> Parse(
            Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence = DefaultPrecedence());
        static BinaryPrecedence NextPrecedence(BinaryPrecedence binaryPrecedence);
        static BinaryOperators StrToOperator(const char *text);
        constexpr inline static BinaryPrecedence DefaultPrecedence() { return (BinaryPrecedence)((int)(BinaryPrecedence::START) + 1); }

    private:
        static Box<Expression> ParseOr(
            Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence);
        static Box<Expression> ParseExpression(
            Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence, std::function<bool(TokenType)> condition);
        static Box<Expression> ParseProduct(
            Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence);

    private:
        Box<Expression> m_Left;
        BinaryOperators m_Operator;
        Box<Expression> m_Right;
    };

    class UnaryExpression : public Expression
    {
    public:
        UnaryExpression(UnaryOperators unaryOperator, Box<Expression> value, UnaryExpressionType expressionType, const Span &span);
        UnaryExpression(UnaryOperators unaryOperator, Box<Expression> value, UnaryExpressionType expressionType, Span &&span);
        ~UnaryExpression() = default;

        inline UnaryOperators Operator() const { return m_UnaryOperator; }
        inline const Expression &Value() const { return *m_Value.get(); }
        inline UnaryExpressionType GetUnaryExpressionType() const { return m_UnaryExpressionType; }

    public:
        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        static Box<Expression> ParsePrefix(Parser &parser, Precedence precedence);
        static std::optional<UnaryOperators> TokenTypeToOperator(TokenType tokenType);

    private:
        UnaryOperators m_UnaryOperator;
        Box<Expression> m_Value;
        UnaryExpressionType m_UnaryExpressionType;
    };

    class CastExpression : public Expression
    {
    public:
        CastExpression(Ref<TypeSpecifier> typeSpecifier, Box<Expression> expression, const Span &span);
        CastExpression(Ref<TypeSpecifier> typeSpecifier, Box<Expression> expression, Span &&span);
        ~CastExpression() = default;

        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        Ref<TypeSpecifier> m_TypeSpecifier;
        Box<Expression> m_Expression;
    };

    class PrimitiveExpression : public Expression
    {
    public:
        PrimitiveExpression(Ref<TypeSpecifier> typeSpecifier, const std::string &value, const Span &span);
        PrimitiveExpression(Ref<TypeSpecifier> typeSpecifier, std::string &&value, Span &&span);
        ~PrimitiveExpression() = default;

        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        static Box<Expression> ParseParenthesis(Parser &parser);

    private:
        Ref<TypeSpecifier> m_TypeSpecifier;
        std::string m_Value;
    };

    class AssignmentExpression : public Expression
    {
    public:
        AssignmentExpression(Box<Expression> variable, Box<Expression> value, const Span &span);
        ~AssignmentExpression() = default;

        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        Box<Expression> m_Variable;
        Box<Expression> m_Value;
    };

    class MemberAccessExpression : public Expression
    {
    public:
        MemberAccessExpression(Box<Expression> objs, TokenType accessType, Box<Expression> property, const Span &span);
        ~MemberAccessExpression() = default;

        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        Box<Expression> m_Object;
        TokenType m_AccessType;
        Box<Expression> m_Property;
    };

    class FunctionCallExpression : public Expression
    {
    public:
        FunctionCallExpression(Box<Expression> fnName, std::vector<Box<Expression>> &&args, const Span &span);
        ~FunctionCallExpression() = default;

        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        Box<Expression> m_FnName;
        std::vector<Box<Expression>> m_Args;
    };

    class ArrayIndexExpression : public Expression
    {
    public:
        ArrayIndexExpression(Box<Expression> array, Box<Expression> index, Box<Expression> secondIndex, const Span &span);
        ~ArrayIndexExpression() = default;

        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        Box<Expression> m_Array;
        Box<Expression> m_Index;
        Box<Expression> m_SecondIndex;
    };

    class NamespaceExpression : public Expression
    {
    public:
        NamespaceExpression(Box<Expression> namespaceExpr, Box<Expression> value, const Span &span);
        ~NamespaceExpression() = default;

        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        Box<Expression> m_Namespace;
        Box<Expression> m_Value;
    };

    class ArrayInitExpression : public Expression
    {
    public:
        ArrayInitExpression(std::vector<Box<Expression>> &&array, size_t size, const Span &span);
        ~ArrayInitExpression() = default;

        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        std::vector<Box<Expression>> m_Array;
        size_t m_Size;
    };

    class ObjectInitExpression : public Expression
    {
    public:
        ObjectInitExpression(Box<Expression> object, std::vector<Box<Expression>> &&fields, const Span &span);
        ~ObjectInitExpression() = default;

        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        Box<Expression> m_Object;
        std::vector<Box<Expression>> m_Fields;
    };

    class FieldExpression : public Expression
    {
    public:
        FieldExpression(Identifier &&name, Box<Expression> value, const Span &span);
        ~FieldExpression() = default;

        static Box<Expression> Parse(Parser &parser);

    private:
        Identifier m_Name;
        Box<Expression> m_Value;
    };

    class IdentifierExpression : public Expression
    {
    public:
        IdentifierExpression(Identifier &&identifier, const Span &span);
        ~IdentifierExpression() = default;

        static Box<Expression> Parse(Parser &parser, Precedence precedence);

    private:
        Identifier m_Identifier;
    };
} // namespace Marble
