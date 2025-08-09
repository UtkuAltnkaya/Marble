#pragma once

#include "Ast/Ast.hpp"
#include "Ast/Generics.hpp"
#include "Ast/TypeSpecifier.hpp"
#include "Utils/Macros.hpp"
#include <functional>
#include <optional>

namespace Marble
{
  class FunctionCallExpression;
  class IdentifierExpression;
  class SymbolNode;
  class BlockSymbolNode;
  class StructOrEnumSymbolNode;
  enum class SymbolAccess;
  enum class ConversionKind;

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
    ArrayIndex,
    FunctionCall,
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
        : Ast{span, AstType::Expression}, m_ExpressionType{expressionType}, m_ValueType{nullptr} {};

    Expression(Span &&span, ExpressionType expressionType)
        : Ast{std::move(span), AstType::Expression}, m_ExpressionType{expressionType}, m_ValueType{nullptr} {};

    virtual ~Expression() = default;
    virtual Box<Expression> Clone() = 0;
    virtual Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) = 0;

    virtual void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) = 0;
    virtual llvm::Value *Address(CodegenContext &codegenContext) { return nullptr; }
    inline virtual bool IsAssignable() const { return false; }

    static Box<Expression> Parse(Parser &parser, Precedence precedence = DefaultPrecedence());
    static Precedence NextPrecedence(Precedence precedence);

    inline Marble::ExpressionType ExpressionType() const { return m_ExpressionType; }
    inline Ref<TypeSpecifier> ValueType() const { return m_ValueType; }
    inline void ValueType(Ref<TypeSpecifier> valueType) { m_ValueType = valueType; }
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
      ASSERT_D(m_ExpressionType == T::StaticType, "Invalid cast in Expression::Into");
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
      ASSERT_D(m_ExpressionType == T::StaticType, "Invalid cast in Expression::Into");
      return static_cast<const T *>(this);
    }

  protected:
    Marble::ExpressionType m_ExpressionType;
    Ref<TypeSpecifier> m_ValueType;
  };

  class BinaryExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::Binary;

    BinaryExpression(Box<Expression> left, BinaryOperators binaryOperator, Box<Expression> right, const Span &span);
    BinaryExpression(const BinaryExpression &obj);
    BinaryExpression(Box<Expression> left, BinaryOperators binaryOperator, Box<Expression> right, Span &&span);
    ~BinaryExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence,
                                 BinaryPrecedence binaryPrecedence = DefaultPrecedence());
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;

    static BinaryPrecedence NextPrecedence(BinaryPrecedence binaryPrecedence);
    static BinaryOperators StrToOperator(const char *text);
    constexpr inline static BinaryPrecedence DefaultPrecedence() { return (BinaryPrecedence)((int)(BinaryPrecedence::START) + 1); }

    inline const Expression &Left() const { return *m_Left.get(); }
    inline BinaryOperators Operator() const { return m_Operator; }
    inline const Expression &Right() const { return *m_Right.get(); }

    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

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
    UnaryExpression(const UnaryExpression &obj);
    ~UnaryExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;
    llvm::Value *Address(CodegenContext &codegenContext) override;
    bool IsAssignable() const override;

    inline UnaryOperators Operator() const { return m_UnaryOperator; }
    inline const Expression &Value() const { return *m_Value.get(); }
    inline UnaryExpressionType GetUnaryExpressionType() const { return m_UnaryExpressionType; }
    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

  private:
    static Box<Expression> ParsePrefix(Parser &parser, Precedence precedence);
    static std::optional<UnaryOperators> TokenTypeToOperator(TokenType tokenType);
    Ref<TypeSpecifier> AnalyzePostFix(SemanticAnalyzer &semanticAnalyzer);
    Ref<TypeSpecifier> AnalyzePrefix(SemanticAnalyzer &semanticAnalyzer);
    Ref<TypeSpecifier> AnalyzeArithmetic(SemanticAnalyzer &semanticAnalyzer);
    Ref<TypeSpecifier> AnalyzeAddress(SemanticAnalyzer &semanticAnalyzer);
    Ref<TypeSpecifier> AnalyzePointer(SemanticAnalyzer &semanticAnalyzer);
    Ref<TypeSpecifier> AnalyzeNot(SemanticAnalyzer &semanticAnalyzer);
    void CheckType(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> expressionType);

    llvm::Value *CodegenArithmetic(CodegenContext &codegenContext);
    llvm::Value *CodegenLogical(CodegenContext &codegenContext);
    llvm::Value *CodegenIncDec(CodegenContext &codegenContext);
    llvm::Value *CodegenAddressOf(CodegenContext &codegenContext);
    llvm::Value *CodegenDereference(CodegenContext &codegenContext);

  private:
    UnaryOperators m_UnaryOperator;
    Box<Expression> m_Value;
    UnaryExpressionType m_UnaryExpressionType;
  };

  class CastExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::Cast;

    CastExpression(Ref<TypeSpecifier> typeSpecifier, Box<Expression> expression, ConversionKind kind, const Span &span);
    CastExpression(Ref<TypeSpecifier> typeSpecifier, Box<Expression> expression, ConversionKind kind, Span &&span);
    CastExpression(const CastExpression &obj);
    ~CastExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;
    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

    inline Ref<TypeSpecifier> GetTypeSpecifier() const { return m_TypeSpecifier; }
    inline const Expression &GetExpression() const { return *m_Expression.get(); }
    inline ConversionKind GetConversionKind() const { return m_Kind; }
    inline void SetConversionKind(ConversionKind kind) { m_Kind = kind; }

  private:
    Ref<TypeSpecifier> m_TypeSpecifier;
    Box<Expression> m_Expression;
    ConversionKind m_Kind;
  };

  class PrimitiveExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::Primitive;

    PrimitiveExpression(Ref<TypeSpecifier> typeSpecifier, const std::string &value, const Span &span);
    PrimitiveExpression(const PrimitiveExpression &obj);
    PrimitiveExpression(Ref<TypeSpecifier> typeSpecifier, std::string &&value, Span &&span);
    ~PrimitiveExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override
    {
      m_ValueType = m_TypeSpecifier;
      return m_ValueType;
    }
    llvm::Value *Codegen(CodegenContext &codegenContext) override;

    inline Ref<TypeSpecifier> GetTypeSpecifier() const { return m_TypeSpecifier; }
    inline const std::string &GetValue() const { return m_Value; }
    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override {}

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
    AssignmentExpression(const AssignmentExpression &obj);
    ~AssignmentExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;

    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

  private:
    void CheckVariableExpressionTypes(SemanticAnalyzer &semanticAnalyzer) const;

  private:
    Box<Expression> m_Variable;
    Box<Expression> m_Value;
  };

  class MemberAccessExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::MemberAccess;

    MemberAccessExpression(Box<Expression> objs, TokenType accessType, Box<Expression> property, const Span &span);
    MemberAccessExpression(const MemberAccessExpression &obj);
    ~MemberAccessExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;
    llvm::Value *Address(CodegenContext &codegenContext) override;
    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;
    bool IsAssignable() const override { return m_Object->IsAssignable(); }

  private:
    const Identifier &FindConcreteName(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> objectType);
    Ref<TypeSpecifier> AnalyzeObject(SemanticAnalyzer &semanticAnalyzer);
    bool AnalyzeProperty(SemanticAnalyzer &semanticAnalyzer, BlockSymbolNode *properties, StructOrEnumSymbolNode *node);
    void AnalyzeIdentifier(SemanticAnalyzer &semanticAnalyzer, BlockSymbolNode *properties, bool &isPublic);
    void AnalyzeMethod(SemanticAnalyzer &semanticAnalyzer, StructOrEnumSymbolNode *node, bool &isPublic);
    void AnalyzeArrayIndex(SemanticAnalyzer &semanticAnalyzer, BlockSymbolNode *properties, bool &isPublic);
    Box<Expression> CreateObjectPointer(StructOrEnumSymbolNode *node);
    Ref<TypeSpecifier> CheckVisibility(SemanticAnalyzer &semanticAnalyzer, bool isPublic);

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
    FunctionCallExpression(const FunctionCallExpression &obj);
    ~FunctionCallExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;

    virtual Box<Expression> Clone() override;

    inline const Expression &FnName() const { return *m_FnName.get(); }
    inline Expression &FnName() { return *m_FnName.get(); }
    inline void FnName(Box<Expression> fnName) { m_FnName = std::move(fnName); }
    inline const Generics *const GetGenerics() { return m_Generics.get(); }
    inline const std::vector<Box<Expression>> &GetArgs() const { return m_Args; }
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;
    void AddArg(Box<Expression> arg, int index);

  private:
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
    ArrayIndexExpression(const ArrayIndexExpression &obj);
    ~ArrayIndexExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;
    llvm::Value *Address(CodegenContext &codegenContext) override;
    bool IsAssignable() const override { return m_Array->IsAssignable(); }

    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

    inline Expression &Array() { return *m_Array; }

  private:
    void AnalyzeIndex(SemanticAnalyzer &semanticAnalyzer, Expression *indexExpression);

  private:
    Box<Expression> m_Array;
    Box<Expression> m_Index;
    Box<Expression> m_SecondIndex;
  };

  class NamespaceExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::NameSpace;

    NamespaceExpression(Box<Expression> namespaceExpr, Box<Generics> generics, Box<Expression> value, const Span &span);
    NamespaceExpression(const NamespaceExpression &obj);
    ~NamespaceExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;

    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;
    const Expression *GetValue() { return m_Value.get(); }

  private:
    void AnalyzeMemberFunction(SemanticAnalyzer &semanticAnalyzer, StructOrEnumSymbolNode *node);
    Ref<TypeSpecifier> AnalyzeEnumField(SemanticAnalyzer &semanticAnalyzer, StructOrEnumSymbolNode *node);

  private:
    Box<Expression> m_Namespace;
    Box<Generics> m_Generics;
    Box<Expression> m_Value;
  };

  class ArrayInitExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::ArrayInit;

    ArrayInitExpression(std::vector<Box<Expression>> &&array, size_t size, const Span &span);
    ArrayInitExpression(const ArrayInitExpression &obj);
    ~ArrayInitExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;
    void CodegenInPlace(CodegenContext &codegenContext, llvm::AllocaInst *alloca);

    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

  private:
    std::vector<Box<Expression>> m_Array;
    size_t m_Size;
  };

  class ObjectInitExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::ObjectInit;

    ObjectInitExpression(Box<Expression> object, Box<Generics> generics, std::vector<Box<Expression>> &&fields, const Span &span);
    ObjectInitExpression(const ObjectInitExpression &obj);
    ~ObjectInitExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;
    void CodegenInPlace(CodegenContext &codegenContext, llvm::AllocaInst *alloca);

    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

  private:
    Box<Expression> m_Object;
    Box<Generics> m_Generics;
    std::vector<Box<Expression>> m_Fields;
  };

  class FieldExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::ObjectField;

    FieldExpression(Identifier &&name, Box<Expression> value, const Span &span);
    FieldExpression(const FieldExpression &obj);
    ~FieldExpression() = default;

    static Box<Expression> Parse(Parser &parser);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;
    virtual Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

  private:
    Identifier m_Name;
    Box<Expression> m_Value;
  };

  class IdentifierExpression : public Expression
  {
  public:
    static constexpr Marble::ExpressionType StaticType = Marble::ExpressionType::Identifier;

    IdentifierExpression(const Identifier &identifier, const Span &span);
    IdentifierExpression(Identifier &&identifier, Span &&span);
    IdentifierExpression(const IdentifierExpression &obj);
    ~IdentifierExpression() = default;

    static Box<Expression> Parse(Parser &parser, Precedence precedence);
    Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType = nullptr) override;
    llvm::Value *Codegen(CodegenContext &codegenContext) override;
    llvm::Value *Address(CodegenContext &codegenContext) override;
    bool IsAssignable() const override { return true; }
    Box<Expression> Clone() override;
    void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override {}
    inline const Identifier &GetIdentifier() const { return m_Identifier; }
    inline Identifier &GetIdentifier() { return m_Identifier; }
    inline void SetId(const std::string &id) { m_Identifier.Id(id); }

  private:
    SymbolNode *FindNode();

  private:
    Identifier m_Identifier;
  };
} // namespace Marble
