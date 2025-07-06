#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ObjectInitExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        const IdentifierExpression *structName = m_Object->TryInto<IdentifierExpression>();
        Identifier name = structName->GetIdentifier();
        if (!structName)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Object name must be identifier expression", true);
        }

        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *structNode = table.Root()->Iter().Struct(name.Id()).Find();
        if (!structNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find the struct", true);
        }

        if (structNode->IsGeneric())
        {
            auto &a = semanticAnalyzer.InstantiateGenerics(name.Id(), m_Generics.get());
            name.Id(a);
            structNode = table.Root()->Iter().Struct(name.Id()).Find();
        }

        size_t size = structNode->Iter().Count(SymbolNodeTypes::StructField);
        if (m_Fields.size() > size)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Too many fields");
        }
        if (m_Fields.size() < size)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Missing fields");
        }
        table.EnterScope(structNode);
        for (auto &field : m_Fields)
        {
            field->Analyze(semanticAnalyzer);
        }
        table.LeaveScope();
        return MakeRef<TypeSpecifier>(name, Span{});
    }

    Ref<TypeSpecifier> FieldExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *currentScope = table.CurrentScope();

        SymbolNode *fieldNode = currentScope->Iter().StructField(m_Name.Id()).Find();
        if (!fieldNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find the struct", true);
        }
        Ref<TypeSpecifier> typeSpecifier = m_Value->Analyze(semanticAnalyzer);

        VariableSymbolNode *variableNode = fieldNode->Into<VariableSymbolNode>();

        if (*typeSpecifier != *variableNode->GetTypeSpecifier())
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Struct type and expression types do not matches");
        }

        return typeSpecifier;
    }

    void ObjectInitExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_Generics)
        {
            m_Generics->SubstituteGenerics(semanticAnalyzer, map);
        }

        m_Object->SubstituteGenerics(semanticAnalyzer, map);
        for (auto &field : m_Fields)
        {
            field->SubstituteGenerics(semanticAnalyzer, map);
        }
    }

    void FieldExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Value->SubstituteGenerics(semanticAnalyzer, map);
    }

    Box<Expression> ObjectInitExpression::Clone()
    {
        return MakeBox<ObjectInitExpression>(*this);
    }

    Box<Expression> FieldExpression::Clone()
    {
        return MakeBox<FieldExpression>(*this);
    }
} // namespace Marble
