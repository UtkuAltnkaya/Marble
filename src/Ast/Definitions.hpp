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
    class SymbolIterator;
    class ImplDefinition;
    class MemberFunctionDefinition;

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
        inline virtual bool IsAnalyzed() const { return m_IsAnalyzed; }
        virtual bool IsExpanded() const { return m_IsExpanded; }

        inline virtual const std::string &GetName() const = 0;
        inline virtual const Identifier &GetIdentifier() const = 0;
        inline virtual void SetName(Box<Identifier> name) = 0;

        virtual Box<Definition> InstantiateWith(
            SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs) { return nullptr; };
        virtual void SubstituteGenerics(
            SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) {};
        virtual llvm::Function *DeclareSignature(CodegenContext &codegenContext) { return nullptr; }

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
        void CheckParametersType(SemanticAnalyzer &analyzer, Ref<TypeSpecifier> paramType, SymbolIterator &iter);

    protected:
        Marble::DefinitionType m_DefinitionType;
        bool m_IsAnalyzed;
        bool m_IsExpanded;
    };

    class FunctionDefinition : public Definition /*, public GenericDefinition*/
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

        llvm::Function *DeclareSignature(CodegenContext &codegenContext) override;
        llvm::Value *Codegen(CodegenContext &codegenContext) override;

        Box<Definition> InstantiateWith(
            SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs) override;
        Box<Definition> Clone() override;

        inline AccessSpecifier GetAccessSpecifier() const { return m_AccessSpecifier; }
        inline const std::string &GetName() const override { return m_FunctionName->Id(); }
        inline const Identifier &GetIdentifier() const override { return *m_FunctionName.get(); }
        inline const Generics *const GetGenerics() const { return m_Generics.get(); }
        inline const std::vector<Box<VariableType>> &GetParams() const { return m_Params; }
        inline const Ref<TypeSpecifier> &GetReturnType() const { return m_ReturnType; }
        inline const Statement &GetBody() const { return *m_Block.get(); }
        bool operator==(const FunctionDefinition &obj) const;

        inline void SetName(Box<Identifier> name) override { m_FunctionName = std::move(name); }
        inline bool IsGeneric() const override { return m_Generics != nullptr; }

    private:
        void SubstituteGenerics(
            SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

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
        inline const Identifier &GetIdentifier() const override { return m_Field->GetIdentifier(); }
        inline const std::string &GetName() const override { return m_Field->GetIdentifier().Id(); }
        inline void SetName(Box<Identifier> name) override { m_Field->SetIdentifier(std::move(name)); }

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<VariableType> m_Field;
    };

    class StructDefinition : public Definition /*,public GenericDefinition */
    {
    public:
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::Struct;

        StructDefinition(AccessSpecifier accessSpecifier, Box<Identifier> structName, Box<Generics> generics, std::vector<Box<StructFieldDefinition>> &&field, const Span &span);
        StructDefinition(const StructDefinition &obj);
        ~StructDefinition() = default;

        static Box<Definition> Parse(Parser &parser, AccessSpecifier accessSpecifier, const Span &span);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        Box<Definition> InstantiateWith(SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs) override;
        Box<Definition> Clone() override;

        inline AccessSpecifier GetAccessSpecifier() const { return m_AccessSpecifier; }
        inline const std::string &GetName() const override { return m_StructName->Id(); }
        inline const Identifier &GetIdentifier() const override { return *m_StructName.get(); }
        inline const Generics *const GetGenerics() const { return m_Generics.get(); }
        inline const std::vector<Box<StructFieldDefinition>> &GetFields() const { return m_Field; }
        inline bool IsGeneric() const override { return m_Generics != nullptr; }
        inline ImplDefinition *GetImplDefinition() const { return m_ImplDefinition; }

        inline void SetName(Box<Identifier> name) override { m_StructName = std::move(name); }
        inline void SetImplDefinition(ImplDefinition *implDefinition) { m_ImplDefinition = implDefinition; }

    private:
        void SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<Identifier> m_StructName;
        Box<Generics> m_Generics;
        std::vector<Box<StructFieldDefinition>> m_Field;
        ImplDefinition *m_ImplDefinition;
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
        inline const Identifier &GetIdentifier() const override { return *m_EnumName.get(); }
        inline const std::vector<Box<Identifier>> &GetFields() const { return m_Fields; }
        inline void SetName(Box<Identifier> name) override { m_EnumName = std::move(name); }

    private:
        AccessSpecifier m_AccessSpecifier;
        Box<Identifier> m_EnumName;
        std::vector<Box<Identifier>> m_Fields;
    };

    class MemberFunctionPrototypeDefinition : public Definition /* ,public GenericDefinition*/
    {
    public:
        friend class MemberFunctionDefinition;
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::MemberFunctionPrototype;

        MemberFunctionPrototypeDefinition(
            AccessSpecifier accessSpecifier,
            Box<VariableType> method,
            Box<Identifier> name,
            Box<Generics> generics,
            std::vector<Box<VariableType>> &&params,
            Ref<TypeSpecifier> returnType,
            const Span &span);
        MemberFunctionPrototypeDefinition(const MemberFunctionPrototypeDefinition &obj);
        ~MemberFunctionPrototypeDefinition() = default;

        static Box<MemberFunctionPrototypeDefinition> Parse(Parser &parser);
        Box<Definition> InstantiateWith(SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs) override;
        void SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;
        Box<Definition> Clone() override;

        inline AccessSpecifier GetAccessSpecifier() const { return m_AccessSpecifier; }
        inline const VariableType *GetMethod() const { return m_Method.get(); }
        inline const std::string &GetName() const override { return m_Name->Id(); }
        inline const Identifier &GetIdentifier() const override { return *m_Name.get(); }
        inline const Generics *const GetGenerics() const { return m_Generics.get(); }
        inline const std::vector<Box<VariableType>> &GetParams() const { return m_Params; }
        inline const Ref<TypeSpecifier> &GetReturnType() const { return m_ReturnType; }
        inline bool IsGeneric() const override { return m_Generics != nullptr; }
        inline void SetName(Box<Identifier> name) override { m_Name = std::move(name); }

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

    class MemberFunctionDefinition : public Definition /*,public GenericDefinition*/
    {
    public:
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::MemberFunction;

        MemberFunctionDefinition(Box<MemberFunctionPrototypeDefinition> prototype, Box<Statement> block, const Span &span);
        MemberFunctionDefinition(const MemberFunctionDefinition &obj);
        ~MemberFunctionDefinition() = default;

        static Box<MemberFunctionDefinition> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        Box<Definition> InstantiateWith(SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs) override;
        void SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;
        Box<Definition> Clone() override;

        inline const MemberFunctionPrototypeDefinition &GetPrototype() const { return *m_Prototype.get(); }
        inline const Statement &GetBody() const { return *m_Block.get(); }
        inline const std::string &GetName() const override { return m_Prototype->GetName(); }
        inline const Identifier &GetIdentifier() const override { return m_Prototype->GetIdentifier(); }
        inline bool IsGeneric() const override { return m_Prototype->IsGeneric(); }
        inline void SetName(Box<Identifier> name) override { m_Prototype->SetName(std::move(name)); }

    private:
        Box<MemberFunctionPrototypeDefinition> m_Prototype;
        Box<Statement> m_Block;
    };

    class ImplDefinition : public Definition /* ,public GenericDefinition*/
    {
    public:
        static constexpr Marble::DefinitionType StaticType = Marble::DefinitionType::Impl;

        ImplDefinition(Ref<TypeSpecifier> implName, Box<Generics> generics, std::vector<Box<MemberFunctionDefinition>> &&memberFunctions, const Span &span);
        ImplDefinition(const ImplDefinition &obj);
        ~ImplDefinition() = default;
        static Box<Definition> Parse(Parser &parser);
        Ref<TypeSpecifier> Analyze(SemanticAnalyzer &semanticAnalyzer) override;
        Box<Definition> InstantiateWith(SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs) override;
        Box<Definition> Clone() override;

        inline const std::string &GetName() const override { return m_ImplName->ToString(); }
        inline const Ref<TypeSpecifier> GetImplName() const { return m_ImplName; }
        inline const Identifier &GetIdentifier() const override { ASSERT_A(false, "Use GetName or GetImplName function"); }
        inline const Generics *const GetGenerics() const { return m_Generics.get(); }
        inline const std::vector<Box<MemberFunctionDefinition>> &GetMemberFunctions() const { return m_MemberFunctions; }
        inline bool IsGeneric() const override { return m_Generics != nullptr; }
        inline void SetImplName(Ref<TypeSpecifier> implName) { m_ImplName = implName; }
        inline void SetName(Box<Identifier> name) override { ASSERT_A(false, "Use SetImplName function"); }

        void CreateSymbol();
        void AddMemberFunction(Box<MemberFunctionDefinition> memberFunction);

    private:
        SymbolNode *HandleRoot(SymbolNode *node);
        void SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map) override;

    private:
        Ref<TypeSpecifier> m_ImplName;
        Box<Generics> m_Generics;
        std::vector<Box<MemberFunctionDefinition>> m_MemberFunctions;
    };

} // namespace Marble
