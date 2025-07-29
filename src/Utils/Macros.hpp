#pragma once

#include <iostream>
#include <cstdlib>

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

#ifndef NDEBUG
#define ASSERT_D(condition, message)                                \
    do                                                              \
    {                                                               \
        if (!(condition))                                           \
        {                                                           \
            std::cerr << "[DEBUG ASSERT FAILED] (" #condition "), " \
                      << "function " << __func__                    \
                      << ", file " << __FILE__                      \
                      << ", line " << __LINE__ << ".\n"             \
                      << "Message: " << message << std::endl;       \
            std::abort();                                           \
        }                                                           \
    } while (false)
#else
#define ASSERT_D(condition, message) ((void)0)
#endif // NDEBUG

#define ASSERT_A(condition, message)                          \
    do                                                        \
    {                                                         \
        if (!(condition))                                     \
        {                                                     \
            std::cerr << "[ASSERT FAILED] (" #condition "), " \
                      << "function: " << __func__             \
                      << ", file: " << __FILE__               \
                      << ", line: " << __LINE__ << "\n"       \
                      << "Message: " << message << std::endl; \
            std::abort();                                     \
        }                                                     \
    } while (false)

#define UNIMPLEMENTED(message)                            \
    do                                                    \
    {                                                     \
        std::cerr << "[UNIMPLEMENTED] "                   \
                  << "function: " << __func__             \
                  << ", file: " << __FILE__               \
                  << ", line: " << __LINE__ << "\n"       \
                  << "Message: " << message << std::endl; \
        std::abort();                                     \
    } while (false)

#define TODO(message)                                     \
    do                                                    \
    {                                                     \
        std::cerr << "[TODO] "                            \
                  << "function: " << __func__             \
                  << ", file: " << __FILE__               \
                  << ", line: " << __LINE__ << "\n"       \
                  << "Message: " << message << std::endl; \
    } while (false)

} // namespace Marble
