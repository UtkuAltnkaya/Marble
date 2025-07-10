#include "Conversion.hpp"

namespace Marble
{
    Conversion::Conversion()
    {
        for (int i = static_cast<int>(Types::START) + 1; i < static_cast<int>(Types::END); i++)
        {
            Types type = static_cast<Types>(i);
            if (TypeSpecifier::IsPrimitive(type))
            {
                m_ConversionMap[{type, type}] = ConversionKind::Identity;
            }

            m_ConversionMap[{Types::Bool, Types::Int}] = ConversionKind::WideningNumeric;
            m_ConversionMap[{Types::Int, Types::Float}] = ConversionKind::WideningNumeric;
            m_ConversionMap[{Types::Int, Types::Double}] = ConversionKind::WideningNumeric;
            m_ConversionMap[{Types::Float, Types::Double}] = ConversionKind::WideningNumeric;

            m_ConversionMap[{Types::Double, Types::Float}] = ConversionKind::NarrowingNumeric;
            m_ConversionMap[{Types::Double, Types::Int}] = ConversionKind::NarrowingNumeric;
            m_ConversionMap[{Types::Float, Types::Int}] = ConversionKind::NarrowingNumeric;
            m_ConversionMap[{Types::Int, Types::Usize}] = ConversionKind::NarrowingNumeric;
        }
    }

    ConversionKind Conversion::CanConvert(Ref<TypeSpecifier> from, Ref<TypeSpecifier> to)
    {
        if (*from == *to)
        {
            return ConversionKind::Identity;
        }
        Types fromType = from->GetType();
        Types toType = to->GetType();

        auto iter = m_ConversionMap.find({fromType, toType});
        if (iter != m_ConversionMap.end())
        {
            return iter->second;
        }
        if (HandleConstConversion(from, to))
        {
            return ConversionKind::ConstConversion;
        }
        if (HandleNullConversion(from, to))
        {
            return ConversionKind::Identity;
        }

        return ConversionKind::None;
    }

    bool Conversion::HandleConstConversion(Ref<TypeSpecifier> from, Ref<TypeSpecifier> to)
    {
        if (from->GetType() == Types::ConstantType)
        {
            return false;
        }
        if (to->GetType() != Types::ConstantType)
        {
            return false;
        }
        return CanConvert(from, to->Constant().TypeSpecifier) != ConversionKind::None;
    }

    bool Conversion::HandleNullConversion(Ref<TypeSpecifier> from, Ref<TypeSpecifier> to)
    {
        return from->GetType() == Types::Null && to->GetType() == Types::Pointer;
    }

} // namespace Marble
