#pragma once

#include <string>
#include <exception>
#include <iostream>
#include <fstream>

namespace Marble
{
    class File
    {
    public:
        File() = default;
        File(const std::string &fileName) : m_FileName{fileName}
        {
            ReadFile();
        }
        ~File() = default;

        inline const std::string &Content() const { return m_Content; };
        inline const std::string &FileName() const { return m_FileName; };

    private:
        void ReadFile()
        {
            std::cout << m_FileName << std::endl;
            std::fstream file(m_FileName);
            if (!file.is_open())
            {
                throw std::runtime_error("Cannot open file");
            }
            m_Content = std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
        }

    protected:
        std::string m_FileName;
        std::string m_Content;
    };

} // namespace Marble
