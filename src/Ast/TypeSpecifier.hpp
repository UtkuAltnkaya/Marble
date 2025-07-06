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
        Int,
        Usize,
        Float,
        Double,
        Char,
        Str,
        Bool,
        Void,
        UserDefine,
        Pointer,
        ArrayType,
        GenericType,
        Null,
        END
    };

    class TypeSpecifier;
    class SemanticAnalyzer;

    struct ArrayType
    {
        Ref<Marble::TypeSpecifier> TypeSpecifier;
        size_t Size;
    };

    struct PointerType
    {
        Ref<Marble::TypeSpecifier> TypeSpecifier;
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
        TypeSpecifier(PointerType pointer, const Span &span);
        TypeSpecifier(GenericType generic, const Span &span);
        ~TypeSpecifier() = default;

    public:
        static Ref<TypeSpecifier> Parse(Parser &parser);

        const Identifier &UserDefine();
        const ArrayType &Array();
        const PointerType &Pointer();
        const GenericType &Generic();
        inline const Types GetType() { return m_Type; }
        inline void SetType(Types type) { m_Type = type; }
        bool IsPrimitive() const;
        bool operator==(const TypeSpecifier &obj) const;
        bool operator!=(const TypeSpecifier &obj) const;
        const std::string &ToString();
        void SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map);

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
        std::variant<PointerType, Identifier, ArrayType, GenericType> m_Variants;
    };

} // namespace Marble
