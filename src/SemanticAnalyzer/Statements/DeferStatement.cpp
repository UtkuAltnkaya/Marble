#include "Ast/Statements.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> DeferStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        m_Expression->Analyze(semanticAnalyzer);
        return TypeSpecifierOk;
    }
    void DeferStatement::SubstituteGenerics(
        SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Expression->SubstituteGenerics(semanticAnalyzer, map);
    }

    Box<Statement> DeferStatement::Clone()
    {
        return MakeBox<DeferStatement>(*this);
    }
} // namespace Marble
