#include <iostream>

#include "Ast/TypeSpecifier.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "Utils/Macros.hpp"

namespace Marble
{
    void TypeSpecifier::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        switch (m_Type)
        {
        case Types::UserDefine:
        {
            auto &userDefine = UserDefineUnchecked();
            auto iter = map.find(*userDefine.Type);
            if (iter == map.end())
            {
                return;
            }
            const Ref<TypeSpecifier> &argType = iter->second;
            m_Span = argType->m_Span;
            m_Type = argType->m_Type;
            m_TypeName = argType->m_TypeName;
            m_Variants = argType->m_Variants;
            break;
        }
        case Types::Pointer:
        {
            auto &ptr = PointerUnchecked();
            ptr.TypeSpecifier->SubstituteGenerics(genericExpander, map);
            break;
        }
        case Types::ArrayType:
        {
            auto &arr = ArrayUnchecked();
            arr.TypeSpecifier->SubstituteGenerics(genericExpander, map);
            break;
        }
        case Types::ConstantType:
        {
            auto &constant = ConstantUnchecked();
            constant.TypeSpecifier->SubstituteGenerics(genericExpander, map);
            break;
        }
        case Types::GenericType:
        {
            auto &generic = GenericUnchecked();
            for (auto &arg : generic.InnerType)
            {
                arg->SubstituteGenerics(genericExpander, map);
            }
            const std::string &name = genericExpander.Expand(generic.OuterType.Id(), generic.InnerType);
            Span span;
            span.Start = generic.OuterType.GetSpan().Start;
            span.End = Position{span.Start.Row, span.Start.Col + name.size(), span.Start.Cursor + name.size()};
            m_Variants = UserDefineType(Identifier(name, span), UserDefineTypeKinds::Struct);
            m_Span = span;
            m_Type = Types::UserDefine;
            break;
        }
        default:
            break;
        }
    }

    Ref<TypeSpecifier> Marble::TypeSpecifier::PassConst(Ref<TypeSpecifier> ts)
    {
        if (ts->m_Type == Types::ConstantType)
        {
            return ts->Constant().TypeSpecifier;
        }
        return ts;
    }

    Ref<TypeSpecifier> TypeSpecifier::ConvertToConst(Ref<TypeSpecifier> ts)
    {
        return MakeRef<TypeSpecifier>(ConstantType{ts}, ts->m_Span);
    }

    bool TypeSpecifier::IsPrimitive() const
    {
        return TypeSpecifier::IsPrimitive(m_Type);
    }

    bool TypeSpecifier::IsPrimitive(Types type)
    {
        return type == Types::Int ||
               type == Types::Usize ||
               type == Types::Float ||
               type == Types::Double ||
               type == Types::Char ||
               type == Types::Bool;
    }

    bool TypeSpecifier::IsIntegerType()
    {
        return m_Type == Types::Int || m_Type == Types::Usize;
    }

    bool TypeSpecifier::IsFloatingPointType()
    {
        return m_Type == Types::Float || m_Type == Types::Double;
    }

    bool TypeSpecifier::operator==(const TypeSpecifier &obj) const
    {
        if (m_Type != obj.m_Type)
        {
            return false;
        }

        if (this->IsPrimitive() && obj.IsPrimitive())
        {
            return true;
        }

        if (m_Type == Types::Pointer)
        {
            const PointerType &tsLeft = PointerUnchecked();
            const PointerType &tsRight = obj.PointerUnchecked();
            return tsLeft.TypeSpecifier->operator==(*tsRight.TypeSpecifier);
        }

        if (m_Type == Types::UserDefine)
        {
            const UserDefineType &tsLeft = UserDefineUnchecked();
            const UserDefineType &tsRight = obj.UserDefineUnchecked();
            return tsLeft.Type == tsRight.Type;
        }

        if (m_Type == Types::ArrayType)
        {
            const ArrayType &tsLeft = ArrayUnchecked();
            const ArrayType &tsRight = obj.ArrayUnchecked();
            return tsLeft.Size == tsRight.Size && tsLeft.TypeSpecifier->operator==(*tsRight.TypeSpecifier);
        }

        if (m_Type == Types::ConstantType)
        {
            const ConstantType &tsLeft = ConstantUnchecked();
            const ConstantType &tsRight = obj.ConstantUnchecked();
            return tsLeft.TypeSpecifier->operator==(*tsRight.TypeSpecifier);
        }

        if (m_Type == Types::GenericType)
        {
            const GenericType &tsLeft = GenericUnchecked();
            const GenericType &tsRight = obj.GenericUnchecked();

            if (tsLeft.OuterType != tsRight.OuterType)
            {
                return false;
            }

            if (tsLeft.InnerType.size() != tsRight.InnerType.size())
            {
                return false;
            }

            for (size_t i = 0; i < tsLeft.InnerType.size(); i++)
            {
                if (tsLeft.InnerType[i]->operator!=(*tsRight.InnerType[i]))
                {
                    return false;
                }
            }
            return true;
        }

        return true;
    }

    bool TypeSpecifier::operator!=(const TypeSpecifier &obj) const
    {
        return !this->operator==(obj);
    }

    const std::string &TypeSpecifier::ToString()
    {
        if (m_TypeName != "")
        {
            return m_TypeName;
        }

        switch (m_Type)
        {
        case Types::Int:
            m_TypeName = "int";
            break;
        case Types::Usize:
            m_TypeName = "usize";
            break;
        case Types::Float:
            m_TypeName = "float";
            break;
        case Types::Double:
            m_TypeName = "double";
            break;
        case Types::Char:
            m_TypeName = "char";
            break;
        case Types::Str:
            m_TypeName = "str";
            break;
        case Types::Bool:
            m_TypeName = "bool";
            break;
        case Types::Void:
            m_TypeName = "void";
            break;
        case Types::Pointer:
        {
            const auto &ptr = PointerUnchecked();
            m_TypeName = ptr.TypeSpecifier->ToString() + "*";
            break;
        }
        case Types::ArrayType:
        {
            const auto &arr = ArrayUnchecked();
            m_TypeName = arr.TypeSpecifier->ToString() + "[" + std::to_string(arr.Size) + "]";
            break;
        }
        case Types::UserDefine:
        {
            const auto &id = UserDefineUnchecked();
            m_TypeName = *id.Type;
            break;
        }
        case Types::GenericType:
        {
            const auto &gen = GenericUnchecked();
            std::string result = gen.OuterType.Id() + "<";
            for (size_t i = 0; i < gen.InnerType.size(); ++i)
            {
                result += gen.InnerType[i]->ToString();
                if (i + 1 < gen.InnerType.size())
                    result += ",";
            }
            result += ">";
            m_TypeName = result;
            break;
        }
        case Types::ConstantType:
        {
            const auto &constant = ConstantUnchecked();
            m_TypeName += "const " + constant.TypeSpecifier->ToString();
        }
        case Types::Null:
            m_TypeName = "null";
            break;
        default:
            UNREACHABLE();
        }
        return m_TypeName;
    }

    Ref<TypeSpecifier> TypeSpecifier::Clone()
    {
        return MakeRef<TypeSpecifier>(*this);
    }
} // namespace Marble
