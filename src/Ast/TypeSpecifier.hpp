#pragma once

#include <variant>
#include "Ast/Identifier.hpp"
#include "Lexer/Token/Token.hpp"
#include "Utils/Memory.hpp"

namespace Marble
{
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
        END
    };

    class TypeSpecifier;

    struct ArrayType
    {
        Ref<TypeSpecifier> TypeSpecifier;
        size_t Size;
    };

    struct PointerType
    {
        Ref<TypeSpecifier> TypeSpecifier;
    };

    struct GenericType
    {
        Identifier OuterType;
        Ref<TypeSpecifier> InnerType;
    };

    class TypeSpecifier : public Ast
    {
    public:
        TypeSpecifier(Types type);
        TypeSpecifier(Types type, const Span &span);
        TypeSpecifier(TypeSpecifier &&obj);
        TypeSpecifier(ArrayType array, const Span &span);
        TypeSpecifier(const Identifier &identifier, const Span &span);
        TypeSpecifier(PointerType pointer, const Span &span);
        TypeSpecifier(GenericType generic, const Span &span);
        ~TypeSpecifier() = default;

    public:
        const Identifier &UserDefine();
        const ArrayType &Array();
        const PointerType &Pointer();
        const GenericType &Generic();
        inline const Types GetType() { return m_Type; }
        inline void SetType(Types type) { m_Type = type; }

    public:
        static Ref<TypeSpecifier> Parse(Parser &parser);

    private:
        static Ref<TypeSpecifier> Primitive(Parser &parser);
        static Ref<TypeSpecifier> UserDefine(Parser &parser);
        static Ref<TypeSpecifier> Complex(Parser &parser, Ref<TypeSpecifier> typeSpecifier);
        static Ref<TypeSpecifier> Array(Parser &parser, Ref<TypeSpecifier> typeSpecifier);
        static Ref<TypeSpecifier> TwoDimensionalArray(Parser &parser, Ref<TypeSpecifier> typeSpecifier, size_t size);
        static Ref<TypeSpecifier> Pointer(Parser &parser, Ref<TypeSpecifier> typeSpecifier);
        static Ref<TypeSpecifier> Generic(Parser &parser, Ref<TypeSpecifier> typeSpecifier);

        static Types GetPrimitive(Parser &parser);

    private:
        Types m_Type;
        std::variant<PointerType, Identifier, ArrayType, GenericType> m_Variants;
    };

} // namespace Marble
