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

        SymbolTable &table = SymbolTable::Get();

        const Identifier &identifier = identifierExpression->GetIdentifier();

        SymbolIterator iter;

        if (auto node = iter.Struct(identifier.Id()); node)
        {
            if (node->IsGeneric())
            {
                const std::string &name = semanticAnalyzer.InstantiateGenerics(identifier.Id(), m_Generics.get());
                node = SymbolIterator().Struct(name);
            }
            table.EnterScope(node);
            m_ValueType = m_Value->Analyze(semanticAnalyzer);
            DeSugar(semanticAnalyzer, node, identifier.Id());
            return m_ValueType;
        }
        if (auto node = iter.Enum(identifier.Id()); node)
        {
            table.EnterScope(node);
            m_ValueType = m_Value->Analyze(semanticAnalyzer);
            if (m_Value->ExpressionType() == ExpressionType::Identifier)
            {
                table.LeaveScope();
            }
            DeSugar(semanticAnalyzer, node, identifier.Id());
            return m_ValueType;
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

    void NamespaceExpression::DeSugar(SemanticAnalyzer &semanticAnalyzer, SymbolNode *node, const std::string &namespaceName)
    {
        // if (m_Value->ExpressionType() != ExpressionType::FunctionCall)
        // {
        //     return;
        // }
        // FunctionCallExpression *fnCallExpression = m_Value->Into<FunctionCallExpression>();
        // IdentifierExpression *identifierExpression = fnCallExpression->FnName().Into<IdentifierExpression>();
        // SymbolNode *fnNode = node->Iter().Function(identifierExpression->GetIdentifier().Id()).Find();
        // ASSERT_D(fnNode != nullptr, "Cannot find function " + identifierExpression->GetIdentifier().Id());
        // const std::string &fnName = semanticAnalyzer.ConvertMethodIntoFunction(static_cast<Definition *>(fnNode->GetAstPtr()), namespaceName, node);
        // identifierExpression->SetId(fnName);
    }

    Box<Expression> NamespaceExpression::Clone()
    {
        return MakeBox<NamespaceExpression>(*this);
    }
} // namespace Marble
