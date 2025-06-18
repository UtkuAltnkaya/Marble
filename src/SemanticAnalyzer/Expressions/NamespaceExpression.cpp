#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "Ast/Identifier.hpp"

namespace Marble
{
    Ref<TypeSpecifier> NamespaceExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        if (m_Namespace->ExpressionType() != ExpressionType::Identifier)
        {
            throw "Identifier expression expected";
        }
        if (m_Value->ExpressionType() != ExpressionType::Identifier && m_Value->ExpressionType() != ExpressionType::FunctionCall)
        {
            throw "Value either can be identifier or function call expression";
        }

        IdentifierExpression *identifierExpression = m_Namespace->Into<IdentifierExpression>();

        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *root = table.Root();

        const Identifier &identifier = identifierExpression->GetIdentifier();

        SymbolIterator iter = root->Iter();

        if (auto node = iter.Struct(identifier.Id()).Find(); node)
        {
            table.EnterScope(node);
            Ref<TypeSpecifier> ts = m_Value->Analyze(semanticAnalyzer);
            table.LeaveScope();
            return ts;
        }
        if (auto node = iter.Enum(identifier.Id()).Find(); node)
        {
            table.EnterScope(node);
            Ref<TypeSpecifier> ts = m_Value->Analyze(semanticAnalyzer);
            table.LeaveScope();
            return ts;
        }
        throw "Cannot find the {} namespace";
    }

    Box<Expression> NamespaceExpression::Clone()
    {
        return MakeBox<NamespaceExpression>(*this);
    }
} // namespace Marble
