#include "Ast/TypeSpecifier.hpp"

namespace Marble
{
    bool TypeSpecifier::IsPrimitive() const
    {
        return m_Type == Types::Int ||
               m_Type == Types::Usize ||
               m_Type == Types::Float ||
               m_Type == Types::Double ||
               m_Type == Types::Char ||
               m_Type == Types::Str ||
               m_Type == Types::Bool;
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

} // namespace Marble
