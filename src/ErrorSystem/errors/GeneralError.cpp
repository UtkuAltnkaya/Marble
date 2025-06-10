#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{
    GeneralError::GeneralError(std::string_view message, const File *const file)
    {
        std::ostringstream oss;

        if (file)
        {
            oss << "\n"
                << file->FileName();
        }
        oss << message << '\n';
        m_Error = oss.str();
    }

} // namespace Marble
