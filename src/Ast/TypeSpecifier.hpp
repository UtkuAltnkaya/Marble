#pragma once

#include <unordered_map>
#include <variant>
#include <vector>
#include "Ast/Identifier.hpp"
#include "Lexer/Token/Token.hpp"
#include "Utils/Memory.hpp"

namespace Marble
{

#define TypeSpecifierOk nullptr;
#define TypeSpecifierVoid MakeRef<TypeSpecifier>(Types::Void, Span{})

    enum class Types
    {
        START,
        Bool,
        Usize,
        Int,
        Float,
        Double,
        Char,
        Str,
        Void,
        UserDefine,
        Pointer,
        ArrayType,
        GenericType,
        ConstantType,
        Null,
        END
    };

    enum class UserDefineTypeKinds
    {
        Struct,
        Enum,
        Undefined,
    };

    class TypeSpecifier;
    class SemanticAnalyzer;
    class CodegenContext;

    struct ArrayType
    {
        Ref<Marble::TypeSpecifier> TypeSpecifier;
        size_t Size;
    };

    struct PointerType
    {
        Ref<Marble::TypeSpecifier> TypeSpecifier;
    };

    struct ConstantType
    {
        Ref<Marble::TypeSpecifier> TypeSpecifier;
    };

    struct UserDefineType
    {
        Identifier Type;
        UserDefineTypeKinds Kind;

        const Identifier &operator*() const { return Type; }
    };

    struct GenericType
    {
        GenericType(const Identifier &outerType, std::vector<Ref<TypeSpecifier>> &&innerType);
        GenericType(const GenericType &obj);
        GenericType &operator=(const GenericType &obj);
        Identifier OuterType;
        std::vector<Ref<TypeSpecifier>> InnerType;
    };

    class TypeSpecifier : public Ast
    {
    public:
        TypeSpecifier(Types type);
        TypeSpecifier(Types type, const Span &span);
        TypeSpecifier(const TypeSpecifier &obj);
        TypeSpecifier(TypeSpecifier &&obj);
        TypeSpecifier(ArrayType array, const Span &span);
        TypeSpecifier(const Identifier &identifier, const Span &span);
        TypeSpecifier(const Identifier &identifier, const Span &span, UserDefineTypeKinds kind);
        TypeSpecifier(PointerType pointer, const Span &span);
        TypeSpecifier(GenericType generic, const Span &span);
        TypeSpecifier(ConstantType constant, const Span &span);
        ~TypeSpecifier() = default;

    public:
        static Ref<TypeSpecifier> Parse(Parser &parser);
        static Ref<TypeSpecifier> PassConst(Ref<TypeSpecifier> ts);
        static Ref<TypeSpecifier> ConvertToConst(Ref<TypeSpecifier> ts);

        const UserDefineType &UserDefine();
        const ArrayType &Array();
        const PointerType &Pointer();
        const GenericType &Generic();
        const ConstantType &Constant();

        const UserDefineType &UserDefineUnchecked() const;
        const ArrayType &ArrayUnchecked() const;
        const PointerType &PointerUnchecked() const;
        const GenericType &GenericUnchecked() const;
        const ConstantType &ConstantUnchecked() const;

        inline const Types GetType() const { return m_Type; }
        inline void SetType(Types type) { m_Type = type; }
        void SetUserDefineTypeKind(UserDefineTypeKinds kind);

        bool IsPrimitive() const;
        static bool IsPrimitive(Types type);
        bool operator==(const TypeSpecifier &obj) const;
        bool operator!=(const TypeSpecifier &obj) const;
        const std::string &ToString();
        void SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map);

        bool IsIntegerType();
        bool IsFloatingPointType();
        Ref<TypeSpecifier> Clone();

        llvm::Type *ToLLVMType(CodegenContext &codegenContext);

    private:
        static Ref<TypeSpecifier> Primitive(Parser &parser);
        static Ref<TypeSpecifier> UserDefine(Parser &parser, Ref<TypeSpecifier> typeSpecifier);
        static Ref<TypeSpecifier> Complex(Parser &parser, Ref<TypeSpecifier> typeSpecifier);
        static Ref<TypeSpecifier> Array(Parser &parser, Ref<TypeSpecifier> typeSpecifier);
        static Ref<TypeSpecifier> TwoDimensionalArray(Parser &parser, Ref<TypeSpecifier> typeSpecifier, size_t size);
        static Ref<TypeSpecifier> Pointer(Parser &parser, Ref<TypeSpecifier> typeSpecifier);
        static Ref<TypeSpecifier> Generic(Parser &parser, Ref<TypeSpecifier> typeSpecifier);

        static Types GetPrimitive(Parser &parser);

    private:
        std::string m_TypeName = "";
        Types m_Type;
        std::variant<PointerType, UserDefineType, ArrayType, GenericType, ConstantType> m_Variants;
    };

} // namespace Marble
