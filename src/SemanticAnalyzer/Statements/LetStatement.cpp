#include "Ast/Statements.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> LetStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        if (m_Value)
        {
            Ref<TypeSpecifier> typeSpecifier = m_Value->Analyze(semanticAnalyzer);
            if (!m_TypeSpecifier)
            {
                m_TypeSpecifier = typeSpecifier;
            }
            else if (*m_TypeSpecifier != *typeSpecifier)
            {
                throw "Miss matched types";
            }
        }
        SymbolNode *node = SymbolTable::GetInstance().CurrentScope();
        node->Insert(m_Identifier->Id(), new VariableSymbolNode{
                                             *this,
                                             node,
                                         });
        return TypeSpecifierOk;
    }

    void LetStatement::SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_TypeSpecifier)
        {
            m_TypeSpecifier->SubstituteGenerics(map);
        }
        m_Value->SubstituteGenerics(map);
    }

    Box<Statement> LetStatement::Clone()
    {
        return MakeBox<LetStatement>(*this);
    }
} // namespace Marble
