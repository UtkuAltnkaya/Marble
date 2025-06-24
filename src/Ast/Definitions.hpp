#pragma once

#include "Ast/Ast.hpp"
#include "Ast/AccessSpecifier.hpp"
#include "Ast/Statements.hpp"
#include "Ast/VariableType.hpp"
#include "Ast/Generics.hpp"
#include "Utils/Macros.hpp"
#include <vector>

// TODO: Add interface or trait definition
namespace Marble
{
    class SymbolNode;

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
        inline virtual bool IsGeneric() const { return false; }
        inline virtual const std::string &GetName() const = 0;
        inline virtual bool IsAnalyzed() const { return m_IsAnalyzed; }
        // TODO
        virtual Box<Definition> Clone() { return nullptr; }

        template <typename T>
        T *TryInto()
        {
            static_assert(std::is_base_of<Definition, T>::value, "Type of paramater must be definition");
            if (T::StaticType != m_DefinitionType)
            {
                return nullptr;
            }
            return static_cast<T *>(this);
        }

        template <typename T>
        T *Into()
        {
            static_assert(std::is_base_of<Definition, T>::value, "Type of paramater must be definition");
            ASSERT_D(m_DefinitionType == T::StaticType, "Invalid cast in Definition::Into");
            return static_cast<T *>(this);
        }

        template <typename T>
        const T *TryInto() const
        {
            static_assert(std::is_base_of<Definition, T>::value, "Type of paramater must be definition");
            if (T::StaticType != m_DefinitionType)
            {
                return nullptr;
            }
            return static_cast<const T *>(this);
        }

        template <typename T>
        const T *Into() const
        {
            static_assert(std::is_base_of<Definition, T>::value, "Type of paramater must be definition");
            ASSERT_D(m_DefinitionType == T::StaticType, "Invalid cast in Definition::Into");
            return static_cast<const T *>(this);
        }

    protected:
        Marble::DefinitionType m_DefinitionType;
        bool m_IsAnalyzed = false;
    };

    class GenericDefinition
    {
    public:
        virtual ~GenericDefinition() = default;
        inline virtual const std::string &GetName() const = 0;
        virtual Box<Definition> InstantiateWith(const std::vector<Ref<TypeSpecifier>> &typeArgs) = 0;

    protected:
        virtual void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) = 0;
    };

    class FunctionDefinition : public Definition, public GenericDefinition
    {
    public:
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::Function;

        FunctionDefinition(
            AccessSpecifier accessSpecifier,
            Box<Identifier> functionName,
            Box<Generics> generics,
            std::vector<Box<VariableType>> &&params,
            Ref<TypeSpecifier> returnType,
            Box<Statement> block,
            const Span &span);
        ~FunctionDefinition() = default;

        FunctionDefinition(const FunctionDefinition &obj);

        static Box<Definition> Parse(Parser &parser, AccessSpecifier accessSpecifier, const Span &span);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        Box<Definition> InstantiateWith(const std::vector<Ref<TypeSpecifier>> &typeArgs) override;
        Box<Definition> Clone() override;

        inline AccessSpecifier GetAccessSpecifier() const { return m_AccessSpecifier; }
        inline const std::string &GetName() const override { return m_FunctionName->Id(); }
        inline const Generics *const GetGenerics() const { return m_Generics.get(); }
        inline const std::vector<Box<VariableType>> &GetParams() const { return m_Params; }
        inline const Ref<TypeSpecifier> &GetReturnType() const { return m_ReturnType; }
        inline const Statement &GetBody() const { return *m_Block.get(); }
        bool operator==(const FunctionDefinition &obj) const;

        inline bool IsGeneric() const override { return m_Generics != nullptr; }

    private:
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

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
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::StructField;

        StructFieldDefinition(AccessSpecifier accessSpecifier, Box<VariableType> field, const Span &span);
        StructFieldDefinition(const StructFieldDefinition &obj);
        ~StructFieldDefinition() = default;

        static Box<StructFieldDefinition> Parse(Parser &parser);
        Box<Definition> Clone() override;

        inline AccessSpecifier GetAccessSpecifier() const { return m_AccessSpecifier; }
        inline const VariableType &GetField() const { return *m_Field.get(); }
        inline const std::string &GetName() const override { return m_Field->GetIdentifier().Id(); }

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<VariableType> m_Field;
    };

    class StructDefinition : public Definition, public GenericDefinition
    {
    public:
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::Struct;

        StructDefinition(AccessSpecifier accessSpecifier, Box<Identifier> structName, Box<Generics> generics, std::vector<Box<StructFieldDefinition>> &&field, const Span &span);
        StructDefinition(const StructDefinition &obj);
        ~StructDefinition() = default;

        static Box<Definition> Parse(Parser &parser, AccessSpecifier accessSpecifier, const Span &span);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        Box<Definition> InstantiateWith(const std::vector<Ref<TypeSpecifier>> &typeArgs) override;
        Box<Definition> Clone() override;

        inline AccessSpecifier GetAccessSpecifier() const { return m_AccessSpecifier; }
        inline const std::string &GetName() const override { return m_StructName->Id(); }
        inline const Generics *const GetGenerics() const { return m_Generics.get(); }
        inline const std::vector<Box<StructFieldDefinition>> &GetFields() const { return m_Field; }
        inline bool IsGeneric() const override { return m_Generics != nullptr; }

    private:
        void SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<Identifier> m_StructName;
        Box<Generics> m_Generics;
        std::vector<Box<StructFieldDefinition>> m_Field;
    };

    class EnumDefinition : public Definition
    {
    public:
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::Enum;

        EnumDefinition(AccessSpecifier accessSpecifier, Box<Identifier> enumName, std::vector<Box<Identifier>> &&fields, const Span &span);
        ~EnumDefinition() = default;

        static Box<Definition> Parse(Parser &parser, AccessSpecifier accessSpecifier, const Span &span);

        inline AccessSpecifier GetAccessSpecifier() const { return m_AccessSpecifier; }
        inline const std::string &GetName() const override { return m_EnumName->Id(); }
        inline const std::vector<Box<Identifier>> &GetFields() const { return m_Fields; }

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<Identifier> m_EnumName;
        std::vector<Box<Identifier>> m_Fields;
    };

    class MemberFunctionPrototypeDefinition : public Definition
    {
    public:
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::MemberFunctionPrototype;

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

        inline AccessSpecifier GetAccessSpecifier() const { return m_AccessSpecifier; }
        inline const VariableType *GetMethod() const { return m_Method.get(); }
        inline const std::string &GetName() const override { return m_Name->Id(); }
        inline const Generics *const GetGenerics() const { return m_Generics.get(); }
        inline const std::vector<Box<VariableType>> &GetParams() const { return m_Params; }
        inline const Ref<TypeSpecifier> &GetReturnType() const { return m_ReturnType; }
        inline bool IsGeneric() const override { return m_Generics != nullptr; }

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
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::MemberFunction;

        MemberFunctionDefinition(Box<MemberFunctionPrototypeDefinition> prototype, Box<Statement> block, const Span &span);
        ~MemberFunctionDefinition() = default;
        static Box<MemberFunctionDefinition> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;

        inline const MemberFunctionPrototypeDefinition &GetPrototype() const { return *m_Prototype.get(); }
        inline const Statement &GetBody() const { return *m_Block.get(); }
        inline const std::string &GetName() const override { return m_Prototype->GetName(); }

    private:
        Box<MemberFunctionPrototypeDefinition> m_Prototype;
        Box<Statement> m_Block;
    };

    class ImplDefinition : public Definition
    {
    public:
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::Impl;

        ImplDefinition(Ref<TypeSpecifier> implName, Box<Generics> generics, std::vector<Box<MemberFunctionDefinition>> &&memberFunctions, const Span &span);
        ~ImplDefinition() = default;
        static Box<Definition> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;

        inline const std::string &GetName() const override { return m_ImplName->ToString(); }
        inline const Ref<TypeSpecifier> GetImplName() const { return m_ImplName; }
        inline const Generics *const GetGenerics() const { return m_Generics.get(); }
        inline const std::vector<Box<MemberFunctionDefinition>> &GetMemberFunctions() const { return m_MemberFunctions; }
        inline bool IsGeneric() const override { return m_Generics != nullptr; }

    private:
        void CreateSymbol();
        SymbolNode *HandleRoot(SymbolNode *node);

    private:
        Ref<TypeSpecifier> m_ImplName;
        Box<Generics> m_Generics;
        std::vector<Box<MemberFunctionDefinition>> m_MemberFunctions;
    };

} // namespace Marble
