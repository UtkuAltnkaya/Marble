#include "Ast/Statements.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    Ref<TypeSpecifier> LetStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        if (m_Value)
        {
            Ref<TypeSpecifier> typeSpecifier = m_Value->Analyze(semanticAnalyzer);
            if (typeSpecifier->GetType() == Types::Null)
            {
                if (!m_TypeSpecifier)
                {
                    ErrorSystem::AddError(semanticAnalyzer, this, "Type must be initalized", true);
                }
                if (m_TypeSpecifier->GetType() != Types::Pointer)
                {
                    ErrorSystem::AddError(semanticAnalyzer, this, "Null only used with pointer type");
                }
            }
            else if (!m_TypeSpecifier)
            {
                m_TypeSpecifier = typeSpecifier;
            }
            else if (*m_TypeSpecifier != *typeSpecifier)
            {
                if (!semanticAnalyzer.TryImplicitConversion(m_Value, typeSpecifier, m_TypeSpecifier))
                {
                    ErrorSystem::AddError(semanticAnalyzer, this, "Miss matched types");
                }
            }
        }
        else
        {
            if (m_TypeSpecifier->GetType() == Types::ConstantType)
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Const variable \"" + m_Identifier->Id() + "\" requires an initializer");
            }
        }
        SymbolNode *node = SymbolTable::Get().CurrentScope();
        node->Insert(new VariableSymbolNode{*this});
        return TypeSpecifierOk;
    }

    void LetStatement::SubstituteGenerics(
        SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_TypeSpecifier)
        {
            m_TypeSpecifier->SubstituteGenerics(semanticAnalyzer, map);
        }
        if (m_Value)
        {
            m_Value->SubstituteGenerics(semanticAnalyzer, map);
        }
    }

    Box<Statement> LetStatement::Clone()
    {
        return MakeBox<LetStatement>(*this);
    }
} // namespace Marble
