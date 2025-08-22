#pragma once

#ifndef NDEBUG

#include "Utils/Derive/Printer.hpp"

namespace Marble
{
    namespace Derive
    {
        class Debug
        {
        public:
            Debug() {}
            virtual ~Debug() = default;
            virtual void Debuggable(Printer &printer) const = 0;
        };

    } // namespace Derive

} // namespace Marble

#define FIELD(x) #x, x
#define DERIVE_DEBUG(ClassName, ...)                                                                  \
public:                                                                                               \
    void Debuggable(Derive::Printer &printer) const override                                          \
    {                                                                                                 \
        printer.BeginObject(#ClassName);                                                              \
        DebugFields(printer, __VA_ARGS__);                                                            \
        printer.EndObject();                                                                          \
    }                                                                                                 \
                                                                                                      \
    friend std::ostream &operator<<(std::ostream &os, const ClassName &obj)                           \
    {                                                                                                 \
        Derive::Printer printer{os};                                                                  \
        obj.Debuggable(printer);                                                                      \
        return os;                                                                                    \
    }                                                                                                 \
                                                                                                      \
private:                                                                                              \
    void DebugFields(Derive::Printer &printer) const {}                                               \
    template <typename First, typename... Rest>                                                       \
    void DebugFields(Derive::Printer &printer, const char *name, First &&first, Rest &&...rest) const \
    {                                                                                                 \
        printer.Field(name, first);                                                                   \
        if constexpr (sizeof...(rest) > 0)                                                            \
        {                                                                                             \
            DebugFields(printer, rest...);                                                            \
        }                                                                                             \
    }
#else

namespace Marble
{
    namespace Derive
    {
        class Printer;

        class Debug
        {
        public:
            Debug() {}
            virtual ~Debug() = default;
            virtual void Debuggable(Printer &printer) const = 0;
        };

    } // namespace Derive

} // namespace Marble

#define FIELD(x) #x, x
#define DERIVE_DEBUG(ClassName, ...)                         \
public:                                                      \
    void Debuggable(Derive::Printer &printer) const override \
    {                                                        \
    }
#endif