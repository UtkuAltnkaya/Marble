#include <iostream>

#include "Ast/TypeSpecifier.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "Utils/Macros.hpp"

namespace Marble
{
    void TypeSpecifier::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        switch (m_Type)
        {
        case Types::UserDefine:
        {
            auto &userDefine = std::get<Identifier>(m_Variants);
            auto iter = map.find(userDefine.Id());
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
            auto &ptr = std::get<PointerType>(m_Variants);
            ptr.TypeSpecifier->SubstituteGenerics(semanticAnalyzer, map);
            break;
        }
        case Types::ArrayType:
        {
            auto &arr = std::get<ArrayType>(m_Variants);
            arr.TypeSpecifier->SubstituteGenerics(semanticAnalyzer, map);
            break;
        }
        case Types::ConstantType:
        {
            auto &constant = std::get<ConstantType>(m_Variants);
            constant.TypeSpecifier->SubstituteGenerics(semanticAnalyzer, map);
            break;
        }
        case Types::GenericType:
        {
            GenericType &gen = std::get<GenericType>(m_Variants);
            for (auto &arg : gen.InnerType)
            {
                arg->SubstituteGenerics(semanticAnalyzer, map);
            }
            const std::string &name = semanticAnalyzer.InstantiateGenerics(gen.OuterType.Id(), gen.InnerType);
            Span span;
            span.Start = gen.OuterType.GetSpan().Start;
            span.End = Position{span.Start.Row, span.Start.Col + name.size(), span.Start.Cursor + name.size()};
            m_Variants = Identifier(name, span);
            m_Span = span;
            m_Type = Types::UserDefine;
            break;
        }
        default:
            break;
        }
    }

    // bool TypeSpecifier::PassNull(Ref<TypeSpecifier> compareType)
    // {
    //     return m_Type == Types::Null && compareType->m_Type == Types::Pointer;
    // }

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
        ;
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
            const PointerType &tsLeft = std::get<PointerType>(m_Variants);
            const PointerType &tsRight = std::get<PointerType>(obj.m_Variants);
            return tsLeft.TypeSpecifier->operator==(*tsRight.TypeSpecifier);
        }

        if (m_Type == Types::UserDefine)
        {
            const Identifier &tsLeft = std::get<Identifier>(m_Variants);
            const Identifier &tsRight = std::get<Identifier>(obj.m_Variants);
            return tsLeft == tsRight;
        }

        if (m_Type == Types::ArrayType)
        {
            const ArrayType &tsLeft = std::get<ArrayType>(m_Variants);
            const ArrayType &tsRight = std::get<ArrayType>(obj.m_Variants);
            return tsLeft.Size == tsRight.Size && tsLeft.TypeSpecifier->operator==(*tsRight.TypeSpecifier);
        }

        if (m_Type == Types::ConstantType)
        {
            const ConstantType &tsLeft = std::get<ConstantType>(m_Variants);
            const ConstantType &tsRight = std::get<ConstantType>(obj.m_Variants);
            return tsLeft.TypeSpecifier->operator==(*tsRight.TypeSpecifier);
        }

        if (m_Type == Types::GenericType)
        {
            const GenericType &tsLeft = std::get<GenericType>(m_Variants);
            const GenericType &tsRight = std::get<GenericType>(obj.m_Variants);

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
            const auto &ptr = std::get<PointerType>(m_Variants);
            m_TypeName = ptr.TypeSpecifier->ToString() + "*";
            break;
        }
        case Types::ArrayType:
        {
            const auto &arr = std::get<ArrayType>(m_Variants);
            m_TypeName = arr.TypeSpecifier->ToString() + "[]";
            break;
        }
        case Types::UserDefine:
        {
            const auto &id = std::get<Identifier>(m_Variants);
            m_TypeName = id.Id();
            break;
        }
        case Types::GenericType:
        {
            const auto &gen = std::get<GenericType>(m_Variants);
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
            const auto &constant = std::get<ConstantType>(m_Variants);
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

} // namespace Marble
