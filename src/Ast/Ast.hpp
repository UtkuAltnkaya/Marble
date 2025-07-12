#pragma once

#include "Lexer/Token/Token.hpp"
#include "Utils/Memory.hpp"

namespace llvm
{
    class Value;
    class Type;
    class AllocaInst;
    class Function;
} // namespace llvm

namespace Marble
{
    class Parser;
    class TypeSpecifier;
    class SemanticAnalyzer;
    class CodegenContext;

    enum class AstType
    {
        Program,
        Definition,
        Statement,
        Expression,
        Identifier,
        TypeSpecifier,
        VariableType,
        Generics,
    };

    class Ast
    {
    public:
        Ast(const Span &span, AstType type) : m_Span{span}, m_Type{type} {}
        Ast(Span &&span, AstType type) : m_Span{std::move(span)}, m_Type{type} {}
        virtual ~Ast() = default;
        virtual Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) { return nullptr; }
        virtual llvm::Value *Codegen(CodegenContext &codegenContext) { return nullptr; }

        inline const Span &GetSpan() const { return m_Span; }
        inline AstType GetAstType() const { return m_Type; }

    protected:
        Span m_Span;
        AstType m_Type;
    };

} // namespace Marble
