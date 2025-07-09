#pragma once

#include <unordered_map>
#include "Ast/TypeSpecifier.hpp"

namespace Marble
{
    enum class ConversionKind
    {
        None,
        Identity,
        WideningNumeric,
        NarrowingNumeric,
        ConstConversion,
        Invalid
    };

    struct PairHash
    {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2> &p) const
        {
            return std::hash<T1>{}(p.first) ^ (std::hash<T2>{}(p.second) << 1);
        }
    };

    class Conversion
    {
    public:
        Conversion();
        ~Conversion() = default;

        ConversionKind CanConvert(Ref<TypeSpecifier> from, Ref<TypeSpecifier> to);

    private:
        bool HandleConstConversion(Ref<TypeSpecifier> from, Ref<TypeSpecifier> to);
        bool HandleNullConversion(Ref<TypeSpecifier> from, Ref<TypeSpecifier> to);

    private:
        std::unordered_map<std::pair<Types, Types>, ConversionKind, PairHash> m_ConversionMap;
    };

} // namespace Marble
