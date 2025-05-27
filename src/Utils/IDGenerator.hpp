#include <random>
#include <string>
#include <sstream>
#include <iomanip>

namespace Marble
{
    class IDGenerator
    {
    public:
        static std::string Generate()
        {
            static std::random_device s_RandomDevice;
            static std::mt19937 s_Generator(s_RandomDevice());
            static std::uniform_int_distribution<uint32_t> s_Distribution(0, 0xFFFFFFFF);

            std::stringstream ss;
            ss << std::hex << std::setfill('0');

            ss << std::setw(8) << s_Distribution(s_Generator);
            ss << std::setw(4) << (s_Distribution(s_Generator) & 0xFFFF);

            return ss.str();
        }
    };

} // namespace Marble
