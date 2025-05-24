#include "Marble/ArgParser.hpp"
#include "ArgParser.hpp"
#include <iostream>

namespace Marble
{

    ArgParserBuilder::ArgParserBuilder(int argc, char const *argv[])
    {
        m_ArgParser.m_Args.reserve(argc - 1);
        for (size_t i = 1; i < argc; i++)
        {
            m_ArgParser.m_Args.emplace_back(argv[i]);
        }
    }

    ArgParserBuilder &ArgParserBuilder::AddFlag(std::string_view flag, std::string_view alias, std::string_view name)
    {
        ArgInfo info{name, ArgType::Flag, ArgValueType::Bool};
        m_ArgParser.m_ArgMap.insert({flag, info});
        m_ArgParser.m_ArgMap.insert({alias, info});
        return *this;
    }

    ArgParserBuilder &ArgParserBuilder::AddValue(std::string_view flag, std::string_view alias, std::string_view name, ArgValueType valueType)
    {
        ArgInfo info{name, ArgType::Value, valueType};
        m_ArgParser.m_ArgMap.insert({flag, info});
        m_ArgParser.m_ArgMap.insert({alias, info});
        return *this;
    }

    ArgParser ArgParserBuilder::Build()
    {
        return m_ArgParser;
    }

    ArgParser::ArgParser()
    {
    }

    void ArgParser::Parse()
    {
        for (size_t i = 0; i < m_Args.size(); i++)
        {
            auto arg = m_Args[i];
            if (arg.at(0) != '-')
            {
                continue;
            }

            auto it = m_ArgMap.find(arg);
            if (it == m_ArgMap.end())
            {
                // TODO: Warn unknown argument
                continue;
            }

            const auto &info = it->second;

            if (info.Type == ArgType::Flag)
            {
                m_ParsedArgs.insert({info.Name, true});
            }
            else
            {
                if (i + 1 < m_Args.size() && !m_Args[i + 1].empty() && m_Args[i + 1][0] != '-')
                {
                    InsertArg(info, m_Args[++i]);
                }
                else
                {
                    throw "Expected value for " + std::string(arg);
                }
            }
        }
    }

    void ArgParser::InsertArg(const ArgInfo &info, std::string_view value)
    {
        switch (info.ValueType)
        {
        case ArgValueType::Bool:
        {
            bool parsedValue = ParseBool(value);
            m_ParsedArgs.insert({info.Name, parsedValue});
            break;
        }
        case ArgValueType::Int:
        {
            int parsedValue = ParseInt(value);
            m_ParsedArgs.insert({info.Name, parsedValue});
            break;
        }
        case ArgValueType::Float:
        {
            float parsedValue = ParseFloat(value);
            m_ParsedArgs.insert({info.Name, parsedValue});
            break;
        }
        case ArgValueType::String:
        {
            m_ParsedArgs.insert({info.Name, std::string(value)});
            break;
        }
        default:
            break;
        }
    }

    int ArgParser::ParseInt(std::string_view value)
    {
        try
        {
            return std::stoi(std::string(value));
        }
        catch (const std::exception &e)
        {
            return 0;
        }
    }

    float ArgParser::ParseFloat(std::string_view value)
    {
        try
        {
            return std::stof(std::string(value));
        }
        catch (const std::exception &e)
        {
            return 0.0;
        }
    }

    bool ArgParser::ParseBool(std::string_view value)
    {
        if (value == "true")
        {
            return true;
        }
        return false;
    }

} // namespace Marble
