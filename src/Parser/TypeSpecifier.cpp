#include "Utils/Macros.hpp"
#include "Ast/TypeSpecifier.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    TypeSpecifier::TypeSpecifier(Types type) : Ast{Span{}, AstType::TypeSpecifier}, m_Type{type}
    {
    }

    TypeSpecifier::TypeSpecifier(Types type, const Span &span) : Ast{span, AstType::TypeSpecifier}, m_Type{type}
    {
    }

    TypeSpecifier::TypeSpecifier(const TypeSpecifier &obj)
        : Ast{obj.m_Span, AstType::TypeSpecifier}, m_Type{obj.m_Type}, m_Variants{obj.m_Variants}
    {
    }

    TypeSpecifier::TypeSpecifier(TypeSpecifier &&obj)
        : Ast{std::move(obj.m_Span), AstType::TypeSpecifier}, m_Type{obj.m_Type}, m_Variants{std::move(obj.m_Variants)}
    {
    }
    TypeSpecifier::TypeSpecifier(ArrayType array, const Span &span)
        : Ast{span, AstType::TypeSpecifier}, m_Type{Types::ArrayType}, m_Variants{array}
    {
    }
    TypeSpecifier::TypeSpecifier(const Identifier &identifier, const Span &span)
        : Ast{span, AstType::TypeSpecifier}, m_Type{Types::UserDefine}, m_Variants{identifier}
    {
    }

    TypeSpecifier::TypeSpecifier(PointerType pointer, const Span &span)
        : Ast{span, AstType::TypeSpecifier}, m_Type{Types::Pointer}, m_Variants{pointer}
    {
    }

    TypeSpecifier::TypeSpecifier(GenericType generic, const Span &span)
        : Ast{span, AstType::TypeSpecifier}, m_Type{Types::GenericType}, m_Variants{generic}
    {
    }
    TypeSpecifier::TypeSpecifier(ConstantType constant, const Span &span)
        : Ast{span, AstType::TypeSpecifier}, m_Type{Types::ConstantType}, m_Variants{constant}
    {
    }

    GenericType::GenericType(const Identifier &outerType, std::vector<Ref<TypeSpecifier>> &&innerType)
        : OuterType{outerType}, InnerType{std::move(innerType)}
    {
    }

    GenericType::GenericType(const GenericType &obj) : OuterType{obj.OuterType}
    {
        for (auto &inner : obj.InnerType)
        {
            InnerType.push_back(MakeRef<TypeSpecifier>(*inner));
        }
    }

    GenericType &GenericType::operator=(const GenericType &obj)
    {
        if (this == &obj)
        {
            return *this;
        }
        OuterType = obj.OuterType;
        InnerType.clear();
        for (auto &inner : obj.InnerType)
        {
            InnerType.push_back(MakeRef<TypeSpecifier>(*inner));
        }
        return *this;
    }

    const Identifier &TypeSpecifier::UserDefine()
    {
        if (std::holds_alternative<Identifier>(m_Variants))
        {
            return std::get<Identifier>(m_Variants);
        }
        ErrorSystem::AddError("Cannot get the user define type");
        UNREACHABLE();
    }

    const ArrayType &TypeSpecifier::Array()
    {
        if (std::holds_alternative<ArrayType>(m_Variants))
        {
            return std::get<ArrayType>(m_Variants);
        }
        ErrorSystem::AddError("Cannot get the array type");
        UNREACHABLE();
    }

    const PointerType &TypeSpecifier::Pointer()
    {
        if (std::holds_alternative<PointerType>(m_Variants))
        {
            return std::get<PointerType>(m_Variants);
        }
        ErrorSystem::AddError("Cannot get the pointer type");
        UNREACHABLE();
    }

    const GenericType &TypeSpecifier::Generic()
    {
        if (std::holds_alternative<GenericType>(m_Variants))
        {
            return std::get<GenericType>(m_Variants);
        }
        ErrorSystem::AddError("Cannot get the generic type");
        UNREACHABLE();
    }

    const ConstantType &TypeSpecifier::Constant()
    {
        if (std::holds_alternative<ConstantType>(m_Variants))
        {
            return std::get<ConstantType>(m_Variants);
        }
        ErrorSystem::AddError("Cannot get the constant type");
        UNREACHABLE();
    }

    Ref<TypeSpecifier> TypeSpecifier::Parse(Parser &parser)
    {
        Ref<TypeSpecifier> result = nullptr;
        Span span;
        bool flag = false;
        if (parser.Current().TokenType() == TokenType::Const)
        {
            flag = true;
            span = parser.Current().Span();
            parser.NextToken();
        }

        auto &token = parser.Current();
        if (token.TokenType() == TokenType::Identifier)
        {
            Ref<TypeSpecifier> typeSpecifier = MakeRef<TypeSpecifier>(Identifier{token}, token.Span());
            result = TypeSpecifier::UserDefine(parser, typeSpecifier);
        }
        else
        {
            result = TypeSpecifier::Primitive(parser);
        }
        if (flag)
        {
            return MakeRef<TypeSpecifier>(ConstantType{result}, Span{span.Start, result->m_Span.Start});
        }
        return result;
    }

    Ref<TypeSpecifier> TypeSpecifier::Primitive(Parser &parser)
    {
        Types primitive = TypeSpecifier::GetPrimitive(parser);
        Ref<TypeSpecifier> typeSpecifier = MakeRef<TypeSpecifier>(primitive, parser.Current().Span());
        return TypeSpecifier::Complex(parser, typeSpecifier);
    }

    Ref<TypeSpecifier> TypeSpecifier::UserDefine(Parser &parser, Ref<TypeSpecifier> typeSpecifier)
    {
        Ref<TypeSpecifier> complex = TypeSpecifier::Complex(parser, typeSpecifier);
        if (typeSpecifier == complex)
        {
            return typeSpecifier;
        }

        return TypeSpecifier::UserDefine(parser, complex);
    }

    Ref<TypeSpecifier> TypeSpecifier::Complex(Parser &parser, Ref<TypeSpecifier> typeSpecifier)
    {
        TokenType tokenType = parser.Next().TokenType();
        if (tokenType == TokenType::LessThan)
        {
            parser.NextToken();
            return TypeSpecifier::Generic(parser, typeSpecifier);
        }
        if (tokenType == TokenType::OpenBracket)
        {
            parser.NextToken(); // Skip type token;
            return TypeSpecifier::Array(parser, typeSpecifier);
        }
        if (tokenType == TokenType::Multiply)
        {
            return TypeSpecifier::Pointer(parser, typeSpecifier);
        }
        return typeSpecifier;
    }

    Ref<TypeSpecifier> TypeSpecifier::Array(Parser &parser, Ref<TypeSpecifier> typeSpecifier)
    {
        parser.NextToken();
        int size = std::stoi(parser.Current().Text());
        if (size <= 0)
        {
            ErrorSystem::AddError(parser, "Array sizes cannot be negative or zero!");
        }
        parser.NextTokenAndExpect(TokenType::CloseBracket);

        if (parser.Next().TokenType() == TokenType::OpenBracket)
        {
            return TypeSpecifier::TwoDimensionalArray(parser, typeSpecifier, static_cast<size_t>(size));
        }

        Span span{typeSpecifier->GetSpan().Start, parser.Current().Span().End};
        return MakeRef<TypeSpecifier>(ArrayType{typeSpecifier, static_cast<size_t>(size)}, span);
    }

    Ref<TypeSpecifier> TypeSpecifier::TwoDimensionalArray(Parser &parser, Ref<TypeSpecifier> typeSpecifier, size_t size)
    {
        const Span end = parser.Current().Span();
        parser.NextToken(); // Skip close bracket

        Ref<TypeSpecifier> array = TypeSpecifier::Array(parser, typeSpecifier);
        const ArrayType &arrayType = array->Array();
        if (arrayType.TypeSpecifier->m_Type == Types::ArrayType)
        {
            ErrorSystem::AddError(parser, "Arrays can only be 1 or 2 dimensional");
        }
        Span span{Span{array->GetSpan().Start, end.Start}};
        return MakeRef<TypeSpecifier>(ArrayType{array, size}, span);
    }

    Ref<TypeSpecifier> TypeSpecifier::Pointer(Parser &parser, Ref<TypeSpecifier> typeSpecifier)
    {
        if (parser.Next().TokenType() != TokenType::Multiply)
        {
            return typeSpecifier;
        }
        Span span{typeSpecifier->m_Span.Start, parser.Next().Span().Start};
        Ref<TypeSpecifier> pointer = MakeRef<TypeSpecifier>(PointerType{typeSpecifier}, span);
        parser.NextToken();
        return TypeSpecifier::Pointer(parser, pointer);
    }

    Ref<TypeSpecifier> TypeSpecifier::Generic(Parser &parser, Ref<TypeSpecifier> typeSpecifier)
    {
        std::vector<Ref<TypeSpecifier>> genericTypes;
        Parenthesis::Parse<Ref<TypeSpecifier>>(genericTypes, parser, TokenType::GreaterThan, [](Parser &parser)
                                               { 
                                                Ref<TypeSpecifier> genericType = TypeSpecifier::Parse(parser);
                                                if (genericType->m_Type == Types::ArrayType)
                                                {
                                                    ErrorSystem::AddError(parser, "Array cannot be argument for generics");
                                                }
                                                return genericType; });

        Span span{typeSpecifier->m_Span.Start, parser.Current().Span().End};

        return MakeRef<TypeSpecifier>(GenericType{typeSpecifier->UserDefine(), std::move(genericTypes)}, span);
    }

    Types TypeSpecifier::GetPrimitive(Parser &parser)
    {
        switch (parser.Current().TokenType())
        {
        case TokenType::Int:
            return Types::Int;
        case TokenType::Usize:
            return Types::Usize;
        case TokenType::Float:
            return Types::Float;
        case TokenType::Double:
            return Types::Double;
        case TokenType::Bool:
            return Types::Bool;
        case TokenType::CharKeyword:
            return Types::Char;
        case TokenType::Str:
            return Types::Str;
        case TokenType::Void:
            return Types::Void;
        default:
            ErrorSystem::AddError(parser, "Unknown TypeSpecifier");
        }
        return Types::END;
    }

} // namespace Marble
