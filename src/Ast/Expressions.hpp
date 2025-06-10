#pragma once

#include "Ast/Ast.hpp"
#include "Ast/Generics.hpp"
#include "Ast/TypeSpecifier.hpp"
#include <functional>
#include <optional>
#include <assert.h>

namespace Marble
{
  class FunctionCallExpression;
  class IdentifierExpression;
  enum class SymbolAccess;

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
    Assignment,
    Binary,
    Cast,
    Unary,
    MemberAccess,
    FunctionCall,
    ArrayIndex,
    NameSpace,
    ArrayInit,
    ObjectInit,
    Identifier,
    Primitive,
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

    template <typename T>
    T *TryInto()
    {
      static_assert(std::is_base_of<Expression, T>::value, "Type of paramater must be expression");
      if (T::StaticType != m_ExpressionType)
      {
        return nullptr;
      }
      return static_cast<T *>(this);
    }

    template <typename T>
    T *Into()
    {
      static_assert(std::is_base_of<Expression, T>::value, "Type of paramater must be expression");
      assert(m_ExpressionType == T::StaticType && "Invalid cast in Expression::Into");
      return static_cast<T *>(this);
    }

    template <typename T>
    const T *TryInto() const
    {
      static_assert(std::is_base_of<Expression, T>::value, "Type of paramater must be expression");
      if (T::StaticType != m_ExpressionType)
      {
        return nullptr;
      }
      return static_cast<const T *>(this);
    }

    template <typename T>
    const T *Into() const
    {
      static_assert(std::is_base_of<Expression, T>::value, "Type of paramater must be expression");
      assert(m_ExpressionType == T::StaticType && "Invalid cast in Expression::Into");
      return static_cast<const T *>(this);
    }

  protected:
    Marble::ExpressionType m_ExpressionType;
  };

  class BinaryExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::Binary;

    BinaryExpression(Box<Expression> left, BinaryOperators binaryOperator, Box<Expression> right, const Span &span);
    BinaryExpression(Box<Expression> left, BinaryOperators binaryOperator, Box<Expression> right, Span &&span);
    ~BinaryExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence,
                                 BinaryPrecedence binaryPrecedence = DefaultPrecedence());
    Ref<TypeSpecifier> Analyze() override;

    static BinaryPrecedence NextPrecedence(BinaryPrecedence binaryPrecedence);
    static BinaryOperators StrToOperator(const char *text);
    constexpr inline static BinaryPrecedence DefaultPrecedence() { return (BinaryPrecedence)((int)(BinaryPrecedence::START) + 1); }

    inline const Expression &Left() const { return *m_Left.get(); }
    inline BinaryOperators Operator() const { return m_Operator; }
    inline const Expression &Right() const { return *m_Right.get(); }

  private:
    static Box<Expression> ParseOr(Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence);
    static Box<Expression> ParseExpression(Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence,
                                           std::function<bool(TokenType)> condition);
    static Box<Expression> ParseProduct(Parser &parser, Precedence precedence, BinaryPrecedence binaryPrecedence);

  private:
    Box<Expression> m_Left;
    BinaryOperators m_Operator;
    Box<Expression> m_Right;
  };

  class UnaryExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::Unary;
    UnaryExpression(UnaryOperators unaryOperator, Box<Expression> value, UnaryExpressionType expressionType,
                    const Span &span);
    UnaryExpression(UnaryOperators unaryOperator, Box<Expression> value, UnaryExpressionType expressionType,
                    Span &&span);
    ~UnaryExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze() override;

    inline UnaryOperators Operator() const { return m_UnaryOperator; }
    inline const Expression &Value() const { return *m_Value.get(); }
    inline UnaryExpressionType GetUnaryExpressionType() const { return m_UnaryExpressionType; }

  private:
    static Box<Expression> ParsePrefix(Parser &parser, Precedence precedence);
    static std::optional<UnaryOperators> TokenTypeToOperator(TokenType tokenType);
    Ref<TypeSpecifier> AnalyzePostFix();
    Ref<TypeSpecifier> AnalyzePrefix();
    Ref<TypeSpecifier> AnalyzeArithmetic();
    Ref<TypeSpecifier> AnalyzeAddress();
    Ref<TypeSpecifier> AnalyzePointer();
    Ref<TypeSpecifier> AnalyzeNot();
    void CheckType(Ref<TypeSpecifier> expressionType);

  private:
    UnaryOperators m_UnaryOperator;
    Box<Expression> m_Value;
    UnaryExpressionType m_UnaryExpressionType;
  };

  class CastExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::Cast;

    CastExpression(Ref<TypeSpecifier> typeSpecifier, Box<Expression> expression, const Span &span);
    CastExpression(Ref<TypeSpecifier> typeSpecifier, Box<Expression> expression, Span &&span);
    ~CastExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze() override
    {
      Ref<TypeSpecifier> ts = m_Expression->Analyze();
      if (!ts->IsPrimitive())
      {
        throw "Cannot cast the complex type";
      }
      return ts;
    }

  private:
    Ref<TypeSpecifier> m_TypeSpecifier;
    Box<Expression> m_Expression;
  };

  class PrimitiveExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::Primitive;

    PrimitiveExpression(Ref<TypeSpecifier> typeSpecifier, const std::string &value, const Span &span);
    PrimitiveExpression(Ref<TypeSpecifier> typeSpecifier, std::string &&value, Span &&span);
    ~PrimitiveExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze() override { return m_TypeSpecifier; }
    inline Ref<TypeSpecifier> GetTypeSpecifier() const { return m_TypeSpecifier; }
    inline const std::string &GetValue() const { return m_Value; }

  private:
    static Box<Expression> ParseParenthesis(Parser &parser);

  private:
    Ref<TypeSpecifier> m_TypeSpecifier;
    std::string m_Value;
  };

  class AssignmentExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::Assignment;

    AssignmentExpression(Box<Expression> variable, Box<Expression> value, const Span &span);
    ~AssignmentExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze() override;

  private:
    void CheckVariableExpressionTypes() const;

  private:
    Box<Expression> m_Variable;
    Box<Expression> m_Value;
  };

  class MemberAccessExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::MemberAccess;

    MemberAccessExpression(Box<Expression> objs, TokenType accessType, Box<Expression> property, const Span &span);
    ~MemberAccessExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze() override;

  private:
    void CheckObjectExpressionType();
    Ref<TypeSpecifier> AnalyzeMethod(FunctionCallExpression *fnCallExpression);
    Ref<TypeSpecifier> AnalyzeIdentifier(IdentifierExpression *identifierExpression);
    void CheckAccessSpecifier(SymbolAccess access);

  private:
    Box<Expression> m_Object;
    TokenType m_AccessType;
    Box<Expression> m_Property;
  };

  class FunctionCallExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::FunctionCall;

    FunctionCallExpression(Box<Expression> fnName, Box<Generics> generics, std::vector<Box<Expression>> &&args,
                           const Span &span);
    ~FunctionCallExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze() override;

    inline const Expression &FnName() const { return *m_FnName.get(); }
    inline const Generics *const GetGenerics() { return m_Generics.get(); }
    inline const std::vector<Box<Expression>> &GetArgs() const { return m_Args; }

  private:
    Box<Expression> m_FnName;
    Box<Generics> m_Generics;
    std::vector<Box<Expression>> m_Args;
  };

  class ArrayIndexExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::ArrayIndex;

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
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::NameSpace;

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
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::ArrayInit;

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
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::ObjectInit;

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
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::ObjectField;

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
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::Identifier;

    IdentifierExpression(Identifier &&identifier, const Span &span);
    ~IdentifierExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    inline const Identifier &GetIdentifier() const { return m_Identifier; }

  private:
    Identifier m_Identifier;
  };
} // namespace Marble
