#pragma once

#include <string_view>
#include <string>

#include <vector>
#include <unordered_map>

#include <optional>
#include <variant>

#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{
    enum class ArgType
    {
        Flag,
        Value
    };

    enum class ArgValueType
    {
        String,
        Int,
        Float,
        Bool,
    };

    struct ArgInfo
    {
        std::string_view Name;
        ArgType Type;
        ArgValueType ValueType;
    };

    class ArgParserBuilder;

    class ArgParser
    {
    public:
        friend class ArgParserBuilder;
        ~ArgParser() = default;

        void Parse();

        template <class T>
        std::optional<T> GetValue(const std::string &name)
        {
            auto it = m_ParsedArgs.find(name);
            if (it == m_ParsedArgs.end())
            {
                return std::nullopt;
            }

            auto &value = it->second;

            if (std::holds_alternative<T>(value))
            {
                return std::get<T>(value);
            }

            ErrorSystem::AddError("Cannot get the requested type", nullptr, true);
            UNREACHABLE();
        }

    private:
        ArgParser();
        void InsertArg(const ArgInfo &info, std::string_view value);
        int ParseInt(std::string_view value);
        float ParseFloat(std::string_view value);
        bool ParseBool(std::string_view value);

    private:
        using ParsedArgInfo = std::variant<bool, float, int, std::string>;

        std::vector<std::string_view> m_Args;
        std::unordered_map<std::string_view, ArgInfo> m_ArgMap;
        std::unordered_map<std::string_view, ParsedArgInfo> m_ParsedArgs;
    };

    class ArgParserBuilder
    {
    public:
        ArgParserBuilder(int argc, char const *argv[]);
        ~ArgParserBuilder() = default;

        ArgParserBuilder &AddFlag(std::string_view flag, std::string_view alias, std::string_view name);
        ArgParserBuilder &AddValue(std::string_view flag, std::string_view alias, std::string_view name, ArgValueType valueType = ArgValueType::String);
        ArgParser Build();

    private:
        ArgParser m_ArgParser;
    };

} // namespace Marble
