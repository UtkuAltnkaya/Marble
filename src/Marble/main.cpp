#include <iostream>
#include "Compiler.hpp"

int main(int argc, char const *argv[])
{
    try
    {
        Marble::Compiler compiler{argc, argv};
        compiler.Compile();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    catch (const std::string &e)
    {
        std::cerr << e << '\n';
    }
    catch (const char *e)
    {
        std::cerr << e << '\n';
    }
    catch (...)
    {
        std::cerr << "Error" << '\n';
    }

    return 0;
}
