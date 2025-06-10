#include <iostream>
#include "Compiler.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

int main(int argc, char const *argv[])
{
    Marble::Compiler compiler{argc, argv};
    try
    {
        compiler.Compile();
    }
    catch (const std::exception &e)
    {
        std::cerr << Marble::ErrorSystem::GetInstance() << "\n";
        std::cerr << e.what() << '\n';
    }
    catch (const std::string &error)
    {
        std::cerr << error << '\n';
    }
    catch (const char *error)
    {
        std::cerr << error << '\n';
    }
    catch (...)
    {
        std::cerr << "Error!" << '\n';
    }
    return 0;
}
