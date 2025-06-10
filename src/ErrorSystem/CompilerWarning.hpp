#pragma once

namespace Marble
{
    class Warning
    {
    public:
        Warning() = default;
        virtual ~Warning() = default;

        virtual const char *warn() const = 0;

    private:
    };

    class CompilerWarning : public Warning
    {
    public:
        CompilerWarning();
        ~CompilerWarning();
    };

} // namespace Marble
