#pragma once
#include <memory>

namespace Marble
{
    template <typename T>
    using Box = std::unique_ptr<T>;
    template <typename T, typename... Args>
    constexpr Box<T> MakeBox(Args &&...args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T, typename... Args>
    constexpr Ref<T> MakeRef(Args &&...args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

} // namespace Marble
