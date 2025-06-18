#include "Ast/Statements.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> DeferStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        m_Expression->Analyze(semanticAnalyzer);
        return TypeSpecifierOk;
    }

    Box<Statement> DeferStatement::Clone()
    {
        return MakeBox<DeferStatement>(*this);
    }
} // namespace Marble
