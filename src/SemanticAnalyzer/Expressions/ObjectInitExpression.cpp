#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ObjectInitExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        IdentifierExpression *structName = m_Object->TryInto<IdentifierExpression>();
        if (!structName)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Object name must be identifier expression", true);
        }

        StructOrEnumSymbolNode *structNode = SymbolIterator().Struct(*structName->GetIdentifier());
        if (!structNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find the struct", true);
        }

        if (structNode->IsGeneric())
        {
            const std::string &expandedName = semanticAnalyzer.InstantiateGenerics(*structName->GetIdentifier(), m_Generics.get());
            structNode = SymbolIterator().Struct(expandedName);
            structName->GetIdentifier().Id(expandedName);
        }

        size_t size = structNode->Block()->Size();
        if (m_Fields.size() > size)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Too many fields");
        }
        if (m_Fields.size() < size)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Missing fields");
        }
        for (auto &field : m_Fields)
        {
            SymbolTable::Get().EnterScope(structNode);
            field->Analyze(semanticAnalyzer);
        }
        m_ValueType = MakeRef<TypeSpecifier>(structName->GetIdentifier(), Span{}, UserDefineTypeKinds::Struct);
        return m_ValueType;
    }

    Ref<TypeSpecifier> FieldExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolNode *currentScope = SymbolTable::Get().CurrentScope();
        VariableSymbolNode *fieldNode = SymbolIterator(currentScope->Block()).Variable(m_Name.Id());
        SymbolTable::Get().LeaveScope();

        if (!fieldNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find the struct", true);
        }
        Ref<TypeSpecifier> valueType = m_Value->Analyze(semanticAnalyzer);
        VariableSymbolNode *variableNode = fieldNode->Into<VariableSymbolNode>();
        Ref<TypeSpecifier> fieldType = variableNode->GetTypeSpecifier();

        if (*valueType != *fieldType)
        {
            if (!semanticAnalyzer.TryImplicitConversion(m_Value, valueType, fieldType))
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Struct type and expression types do not matches");
            }
        }
        m_ValueType = valueType;
        return m_ValueType;
    }

    void ObjectInitExpression::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_Generics)
        {
            m_Generics->SubstituteGenerics(genericExpander, map);
        }

        m_Object->SubstituteGenerics(genericExpander, map);
        for (auto &field : m_Fields)
        {
            field->SubstituteGenerics(genericExpander, map);
        }
    }

    void FieldExpression::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Value->SubstituteGenerics(genericExpander, map);
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
