#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerError.hpp"
#include "Ast/Identifier.hpp"

namespace Marble
{
    Ref<TypeSpecifier> NamespaceExpression::Analyze(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> baseType)
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
        std::string name = *identifierExpression->GetIdentifier();

        if (m_Generics)
        {
            name = semanticAnalyzer.InstantiateGenerics(*identifierExpression->GetIdentifier(), m_Generics.get(), this);
        }

        SymbolIterator iter;
        if (auto node = iter.Struct(name); node)
        {
            AnalyzeMemberFunction(semanticAnalyzer, node);
            m_ValueType = m_Value->Analyze(semanticAnalyzer);
            return m_ValueType;
        }
        if (auto node = iter.Enum(name); node)
        {
            m_ValueType = AnalyzeEnumField(semanticAnalyzer, node);
            if (!m_ValueType)
            {
                AnalyzeMemberFunction(semanticAnalyzer, node);
                m_ValueType = m_Value->Analyze(semanticAnalyzer);
            }
            return m_ValueType;
        }
        ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find the namespace", true);
        UNREACHABLE();
    }

    void NamespaceExpression::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_Generics)
        {
            m_Generics->SubstituteGenerics(genericExpander, map);
        }
        m_Namespace->SubstituteGenerics(genericExpander, map);
        m_Value->SubstituteGenerics(genericExpander, map);
    }

    void NamespaceExpression::AnalyzeMemberFunction(SemanticAnalyzer &semanticAnalyzer, StructOrEnumSymbolNode *node)
    {
        if (m_Value->ExpressionType() != ExpressionType::FunctionCall)
        {
            return;
        }
        FunctionCallExpression *fnCallExpression = m_Value->Into<FunctionCallExpression>();
        IdentifierExpression *identifierExpression = fnCallExpression->FnName().Into<IdentifierExpression>();
        auto fnName = node->LookFunctionName(*identifierExpression->GetIdentifier());
        if (!fnName)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find related function name with given method name", true);
        }
        identifierExpression->SetId(*fnName);
    }

    Ref<TypeSpecifier> NamespaceExpression::AnalyzeEnumField(SemanticAnalyzer &semanticAnalyzer, StructOrEnumSymbolNode *node)
    {
        IdentifierExpression *identifierExpr = nullptr;
        if (identifierExpr = m_Value->TryInto<IdentifierExpression>(); !identifierExpr)
        {
            return nullptr;
        }
        BlockSymbolNode *fields = node->Block();

        VariableSymbolNode *variable = SymbolIterator(fields).Variable(*identifierExpr->GetIdentifier());
        if (!variable)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find enum field");
        }
        Identifier name(node->GetSymbolData().Name(), Span{});
        return MakeRef<TypeSpecifier>(name, Span{}, UserDefineTypeKinds::Enum);
    }

    Box<Expression> NamespaceExpression::Clone()
    {
        return MakeBox<NamespaceExpression>(*this);
    }
} // namespace Marble
