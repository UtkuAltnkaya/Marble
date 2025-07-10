#pragma once

#include <string>
#include "ErrorSystem/MessageHolder.hpp"
namespace Marble
{
    class Warning
    {
    public:
        Warning() = default;
        virtual ~Warning() = default;
        virtual const char *warn() const = 0;
    };

    class CompilerWarning : public Warning, public virtual MessageHolder
    {
    public:
        CompilerWarning() = default;
        virtual ~CompilerWarning() = default;
        const char *warn() const noexcept override { return m_Message.c_str(); }
    };

} // namespace Marble
