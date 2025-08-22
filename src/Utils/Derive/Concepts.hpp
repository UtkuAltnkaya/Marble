#pragma once

static_assert(__cplusplus >= 202002L, "C++20 required");

#include <iostream>
#include <type_traits>
#include <concepts>
#include <variant>
#include <memory>
#include <optional>

namespace Marble
{
    namespace Derive
    {
        class Debug;

        template <typename T>
        concept Streamable = requires(std::ostream &os, const T &v) {
            { os << v } -> std::same_as<std::ostream &>;
        };

        template <typename T>
        concept StringLike = std::same_as<T, std::string> ||
                             std::same_as<T, std::string_view> ||
                             std::same_as<T, char *> ||
                             std::same_as<T, const char *>;

        template <typename T>
        concept Iterable = requires(const T &t) {
            { std::begin(t) };
            { std::end(t) };
        };

        template <typename T>
        concept OptionalLike = requires(const T &t) {
            { t.has_value() } -> std::same_as<bool>;
            { *t };
        };

        template <typename T>
        concept PointerLike = requires(const T &t) {
            typename T::element_type;
            { t.get() } -> std::convertible_to<typename T::element_type *>;
            { static_cast<bool>(t) } -> std::same_as<bool>;
        } || (std::is_pointer_v<T>);

        template <typename T>
        concept Enumerable = std::is_enum_v<T>;

        template <typename T>
        concept TupleLike = requires { std::tuple_size<T>::value; };

        template <typename T>
        concept Debuggable = std::is_base_of_v<Debug, T>;

    } // namespace Derive
} // namespace Marble
