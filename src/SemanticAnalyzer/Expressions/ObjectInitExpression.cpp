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

        StructOrEnumSymbolNode *structNode = SymbolIterator().Struct(name.Id());
        if (!structNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find the struct", true);
        }

        if (structNode->IsGeneric())
        {
            TODO("Object init struct generics");
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
        m_ValueType = MakeRef<TypeSpecifier>(name, Span{}, UserDefineTypeKinds::Struct);
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
