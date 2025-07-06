#include <iomanip>
#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{
    static void HandleContent(const Lexer &lexer, std::ostringstream &oss, std::string_view message);
    static size_t CalculatePrintline(const Lexer &lexer, std::ostringstream &oss);
    static void PrintContent(const Lexer &lexer, std::ostringstream &oss);
    static void PrintPoints(const Lexer &lexer, std::ostringstream &oss, size_t byte);

    LexicalError::LexicalError(const Lexer &lexer, std::string_view message)
    {
        std::ostringstream oss;
        HandleContent(lexer, oss, message);
        m_Error = oss.str();
    }

    void HandleContent(const Lexer &lexer, std::ostringstream &oss, std::string_view message)
    {
        oss << "\n"
            << lexer.File().FileName() << ":" << lexer.Line() + 1 << ":"
            << lexer.Cursor() - lexer.LineStart() << "\n\n";

        if (static_cast<int>(lexer.Line()) - 1 >= 0)
        {
            oss << std::setw(2) << std::right << lexer.Line() - 1 << " |\n";
            oss << std::setw(2) << std::right << lexer.Line() << " |\n";
        }

        size_t byte = CalculatePrintline(lexer, oss);
        PrintContent(lexer, oss);
        PrintPoints(lexer, oss, byte);

        oss << std::setw(5) << std::right << " |\t" << message << "\n";
        oss << std::setw(2) << std::right << lexer.Line() + 2 << " |\n";
    }

    size_t CalculatePrintline(const Lexer &lexer, std::ostringstream &oss)
    {
        oss << "   |\t";
        std::string item = std::to_string(lexer.Line() + 1) + "-" +
                           std::to_string(lexer.Cursor() - lexer.LineStart()) + ":  ";
        oss << item;
        return item.size();
    }

    void PrintContent(const Lexer &lexer, std::ostringstream &oss)
    {
        size_t i = lexer.File().Content()[lexer.LineStart()] == '\n' ? lexer.LineStart() + 1 : lexer.LineStart();
        std::string item;
        const std::string &content = lexer.File().Content();
        while (i != content.size() && content[i] != ';' && content[i + 1] != '\n' && content[i] != 0)
        {
            item.push_back(content[i]);
            i++;
        }
        item.push_back(content[i]);
        oss << item << "\n";
    }

    void PrintPoints(const Lexer &lexer, std::ostringstream &oss, size_t byte)
    {
        std::string item("   |\t");
        std::string space(lexer.CursorStart() - lexer.LineStart() + byte - 1, ' ');
        item.append(space);
        if (lexer.Cursor() == lexer.CursorStart())
        {
            item.push_back('^');
        }
        else
        {
            for (size_t i = 0; i < lexer.Cursor() - lexer.CursorStart(); i++)
            {
                item.push_back('^');
            }
        }
        oss << item << "\n";
    }

} // namespace Marble
