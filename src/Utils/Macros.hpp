#pragma once

namespace Marble
{
#if defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE() __assume(0)
#else
#include <cstdlib>
#define UNREACHABLE() std::abort()
#endif
} // namespace Marble
