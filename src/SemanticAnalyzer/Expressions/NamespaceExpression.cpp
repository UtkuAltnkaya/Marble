#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerError.hpp"
#include "Ast/Identifier.hpp"

namespace Marble
{
    Ref<TypeSpecifier> NamespaceExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        if (m_Namespace->ExpressionType() != ExpressionType::Identifier)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Identifier expression expected", true);
        }
        if (m_Value->ExpressionType() != ExpressionType::Identifier && m_Value->ExpressionType() != ExpressionType::FunctionCall)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Value either can be identifier or function call expression", true);
        }

        IdentifierExpression *identifierExpression = m_Namespace->Into<IdentifierExpression>();

        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *root = table.Root();

        const Identifier &identifier = identifierExpression->GetIdentifier();

        SymbolIterator iter = root->Iter();

        if (auto node = iter.Struct(identifier.Id()).Find(); node)
        {
            if (node->IsGeneric())
            {
                const std::string &name = semanticAnalyzer.InstantiateGenerics(identifier.Id(), m_Generics.get());
                node = root->Iter().Struct(name).Find();
            }
            table.EnterScope(node);
            Ref<TypeSpecifier> ts = m_Value->Analyze(semanticAnalyzer);
            return ts;
        }
        if (auto node = iter.Enum(identifier.Id()).Find(); node)
        {
            table.EnterScope(node);
            Ref<TypeSpecifier> ts = m_Value->Analyze(semanticAnalyzer);
            if (m_Value->ExpressionType() == ExpressionType::Identifier)
            {
                table.LeaveScope();
            }
            return ts;
        }
        ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find the namespace", true);
        UNREACHABLE();
    }

    void NamespaceExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_Generics)
        {
            m_Generics->SubstituteGenerics(semanticAnalyzer, map);
        }
        m_Namespace->SubstituteGenerics(semanticAnalyzer, map);
        m_Value->SubstituteGenerics(semanticAnalyzer, map);
    }

    Box<Expression> NamespaceExpression::Clone()
    {
        return MakeBox<NamespaceExpression>(*this);
    }
} // namespace Marble
