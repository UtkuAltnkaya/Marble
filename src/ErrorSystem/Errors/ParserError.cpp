#include <iomanip>
#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{
    static void HandleContent(const Parser &parser, std::ostringstream &oss, std::string_view message);
    static size_t CalculatePrintline(const Parser &parser, std::ostringstream &oss);
    static void PrintContent(const Parser &parser, std::ostringstream &oss);
    static void PrintPoints(const Parser &parser, std::ostringstream &oss, size_t byte);
    static size_t InitContentStart(const Parser &parser);

    ParserError::ParserError(const Parser &parser, std::string_view message)
    {
        std::ostringstream oss;
        HandleContent(parser, oss, message);
        m_Error = oss.str();
    }

    void HandleContent(const Parser &parser, std::ostringstream &oss, std::string_view message)
    {
        oss << "\n"
            << parser.File().FileName() << ":" << parser.Current().Span().Start.Row << ":"
            << parser.Current().Span().Start.Col << "\n\n";

        if (static_cast<int>(parser.Current().Span().Start.Row) - 3 >= 0)
        {
            oss << std::setw(2) << std::right << parser.Current().Span().Start.Row - 2 << " |\n";
            oss << std::setw(2) << std::right << parser.Current().Span().Start.Row - 1 << " |\n";
        }
        size_t byte = CalculatePrintline(parser, oss);
        PrintContent(parser, oss);
        PrintPoints(parser, oss, byte);
        oss << std::setw(5) << std::right << " |\t" << message << "\n";
        oss << std::setw(2) << std::right << parser.Current().Span().Start.Row + 1 << " |\n";
    }

    size_t CalculatePrintline(const Parser &parser, std::ostringstream &oss)
    {
        oss << "   |\t";
        std::string item = std::to_string(parser.Current().Span().Start.Row) + "-" +
                           std::to_string(parser.Current().Span().Start.Col) + ":  ";
        oss << item;
        return item.size();
    }

    void PrintContent(const Parser &parser, std::ostringstream &oss)
    {
        size_t i = InitContentStart(parser) + 1;
        std::string item;
        const std::string &content = parser.File().Content();
        while (i != content.size() && content[i + 1] != '\n' && content[i] != '\0')
        {
            item.push_back(content[i]);
            i++;
        }
        item.push_back(content[i]);
        oss << item << "\n";
    }

    void PrintPoints(const Parser &parser, std::ostringstream &oss, size_t byte)
    {
        std::string item("   |\t");
        std::string space(parser.Current().Span().Start.Col + byte - 1, ' ');
        item += space;

        for (size_t i{0}; i < parser.Current().Text().size(); i++)
        {
            item.push_back('^');
        }
        oss << item << "\n";
    }

    size_t InitContentStart(const Parser &parser)
    {
        const std::string &content = parser.File().Content();
        size_t cursor = parser.Current().Span().Start.Cursor;
        while (content[cursor] != '\n')
        {
            cursor--;
        }

        return cursor;
    }
} // namespace Marble
