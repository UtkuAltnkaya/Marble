#include "Ast/Program.hpp"
#include "Parser/Parser.hpp"

namespace Marble
{
    Program::Program(std::vector<Box<Definition>> &&definitions, const Span &span)
        : Ast{span, AstType::Program}, m_Definitions{std::move(definitions)}
    {
    }

    Ref<Program> Program::Parse(Parser &parser)
    {
        std::vector<Box<Definition>> definitions;

        Span start = parser.Current().Span();
        while (parser.Current().TokenType() != TokenType::Eof)
        {
            definitions.push_back(Definition::Parse(parser));
            parser.NextToken();
        }
        const Span &end = parser.Current().Span();
        return MakeRef<Program>(std::move(definitions), Span{start.Start, end.Start});
    }

} // namespace Marble
