#include <iomanip>
#include <cmath>
#include "ErrorSystem/CompilerError.hpp"

namespace Marble
{
    static void HandleContent(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::ostringstream &oss, std::string_view message);
    static size_t CalculatePrintline(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::ostringstream &oss);
    static void PrintContent(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::ostringstream &oss, size_t byte);
    static void PrintPoints(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::ostringstream &oss, size_t byte);

    SemanticError::SemanticError(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::string_view message)
    {
        std::ostringstream oss;
        HandleContent(semanticAnalyzer, node, oss, message);
        m_Error = oss.str();
    }

    void HandleContent(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::ostringstream &oss, std::string_view message)
    {
        const Span &span = node->GetSpan();
        oss << "\n"
            << semanticAnalyzer.File().FileName() << ":" << span.Start.Row + 1
            << ":" << span.Start.Col << "\n\n";

        if (static_cast<int>(span.Start.Row) - 3 >= 0)
        {
            oss << std::setw(2) << std::right << span.Start.Row - 2 << " |\n";
            oss << std::setw(2) << std::right << span.Start.Row - 1 << " |\n";
        }
        size_t byte = CalculatePrintline(semanticAnalyzer, node, oss);
        PrintContent(semanticAnalyzer, node, oss, byte);
        PrintPoints(semanticAnalyzer, node, oss, byte);
        oss << std::setw(5) << std::right << " |\t" << message << "\n";
        oss << std::setw(2) << std::right << span.End.Row + 1 << " |\n";
    }

    size_t CalculatePrintline(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::ostringstream &oss)
    {
        const Span &span = node->GetSpan();
        oss << "   |\t";
        std::string item = std::to_string(span.Start.Row + 1) + "-" +
                           std::to_string(span.Start.Col) + ":  ";
        oss << item;
        return item.size();
    }

    void PrintContent(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::ostringstream &oss, size_t byte)
    {
        std::string item;
        const Span &span = node->GetSpan();
        const std::string &content = semanticAnalyzer.File().Content();
        size_t i = 0;
        for (i = span.Start.Cursor; i < span.End.Cursor; i++)
        {
            item += content[i];
            if (content[i] == '\n')
            {
                item += "   |\t" + std::string(byte, ' ');
            }
        }
        item.push_back(content[i]);
        oss << item << "\n";
    }

    void PrintPoints(const SemanticAnalyzer &semanticAnalyzer, const Ast *node, std::ostringstream &oss, size_t byte)
    {
        const Span &span = node->GetSpan();
        std::string item("   |\t");
        std::string space(byte, ' ');
        item += space;

        int r = std::min(30, (int)(span.End.Cursor - span.Start.Cursor));
        for (size_t i{0}; i < r; i++)
        {
            item.push_back('^');
        }
        oss << item << "\n";
    }

} // namespace Marble
