#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{
    GeneralError::GeneralError(std::string_view message, const File *const file)
    {
        std::ostringstream oss;

        if (file)
        {
            oss << "\n"
                << file->FileName() << "\n";
        }
        oss << message << '\n';
        m_Message = oss.str();
    }

} // namespace Marble
