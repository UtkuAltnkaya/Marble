#pragma once

#include "Ast/Ast.hpp"
#include "Ast/AccessSpecifier.hpp"
#include "Ast/Statements.hpp"
#include "Ast/VariableType.hpp"
#include "Ast/Generics.hpp"
#include <vector>

// TODO: Parse generic type definition
namespace Marble
{
    enum class DefinitionType
    {
        Function,
        Struct,
        StructField,
        Enum,
        Impl,
        MemberFunctionPrototype,
        MemberFunction,
    };

    class Definition : public Ast
    {
    public:
        Definition(const Span &span, DefinitionType definitionType);
        Definition(Span &&span, DefinitionType definitionType);
        virtual ~Definition() = default;

        static Box<Definition> Parse(Parser &parser);
        inline Marble::DefinitionType DefinitionType() const { return m_DefinitionType; }

    protected:
        Marble::DefinitionType m_DefinitionType;
    };

    class FunctionDefinition : public Definition
    {
    public:
        FunctionDefinition(
            AccessSpecifier accessSpecifier,
            Box<Identifier> functionName,
            Box<Generics> generics,
            std::vector<Box<VariableType>> &&params,
            Ref<TypeSpecifier> returnType,
            Box<Statement> block,
            const Span &span);
        ~FunctionDefinition() = default;

        static Box<Definition> Parse(Parser &parser, AccessSpecifier accessSpecifier, const Span &span);

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<Identifier> m_FunctionName;
        Box<Generics> m_Generics;
        std::vector<Box<VariableType>> m_Params;
        Ref<TypeSpecifier> m_ReturnType;
        Box<Statement> m_Block;
    };

    class StructFieldDefinition : public Definition
    {
    public:
        StructFieldDefinition(AccessSpecifier accessSpecifier, Box<VariableType> field, const Span &span);
        ~StructFieldDefinition() = default;

        static Box<StructFieldDefinition> Parse(Parser &parser);

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<VariableType> m_Field;
    };

    class StructDefinition : public Definition
    {
    public:
        StructDefinition(AccessSpecifier accessSpecifier, Box<Identifier> structName, Box<Generics> generics, std::vector<Box<StructFieldDefinition>> &&field, const Span &span);
        ~StructDefinition() = default;

        static Box<Definition> Parse(Parser &parser, AccessSpecifier accessSpecifier, const Span &span);

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<Identifier> m_StructName;
        Box<Generics> m_Generics;
        std::vector<Box<StructFieldDefinition>> m_Field;
    };

    class EnumDefinition : public Definition
    {
    public:
        EnumDefinition(AccessSpecifier accessSpecifier, Box<Identifier> enumName, std::vector<Box<Identifier>> &&fields, const Span &span);
        ~EnumDefinition() = default;

        static Box<Definition> Parse(Parser &parser, AccessSpecifier accessSpecifier, const Span &span);

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<Identifier> m_EnumName;
        std::vector<Box<Identifier>> m_Fields;
    };

    class MemberFunctionPrototypeDefinition : public Definition
    {
    public:
        MemberFunctionPrototypeDefinition(
            AccessSpecifier accessSpecifier,
            Box<VariableType> method,
            Box<Identifier> name,
            Box<Generics> generics,
            std::vector<Box<VariableType>> &&params,
            Ref<TypeSpecifier> returnType,
            const Span &span);

        ~MemberFunctionPrototypeDefinition() = default;
        static Box<MemberFunctionPrototypeDefinition> Parse(Parser &parser);

    private:
        static Box<VariableType> ParseMethod(Parser &parser);

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<VariableType> m_Method;
        Box<Identifier> m_Name;
        Box<Generics> m_Generics;
        std::vector<Box<VariableType>> m_Params;
        Ref<TypeSpecifier> m_ReturnType;
    };

    class MemberFunctionDefinition : public Definition
    {
    public:
        MemberFunctionDefinition(Box<MemberFunctionPrototypeDefinition> prototype, Box<Statement> block, const Span &span);
        ~MemberFunctionDefinition() = default;
        static Box<MemberFunctionDefinition> Parse(Parser &parser);

    private:
        Box<MemberFunctionPrototypeDefinition> m_Prototype;
        Box<Statement> m_Block;
    };

    class ImplDefinition : public Definition
    {
    public:
        ImplDefinition(Ref<TypeSpecifier> implName, Box<Generics> generics, std::vector<Box<MemberFunctionDefinition>> &&memberFunctions, const Span &span);
        ~ImplDefinition() = default;
        static Box<Definition> Parse(Parser &parser);

    private:
        Ref<TypeSpecifier> m_ImplName;
        Box<Generics> m_Generics;
        std::vector<Box<MemberFunctionDefinition>> m_MemberFunctions;
    };

} // namespace Marble
