#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ObjectInitExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        const IdentifierExpression *structName = m_Object->TryInto<IdentifierExpression>();
        if (!structName)
        {
            throw "Object name must be identifier expression";
        }

        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *structNode = table.Root()->Iter().Struct(structName->GetIdentifier().Id()).Find();
        if (!structNode)
        {
            throw "Cannot find the struct";
        }
        size_t size = structNode->Iter().Count(SymbolNodeTypes::StructField);
        if (m_Fields.size() > size)
        {
            throw "Too many fields";
        }
        if (m_Fields.size() < size)
        {
            throw "Missing fields";
        }
        table.EnterScope(structNode);
        for (auto &field : m_Fields)
        {
            field->Analyze(semanticAnalyzer);
        }
        table.LeaveScope();
        return MakeRef<TypeSpecifier>(structName->GetIdentifier(), Span{});
    }

    Ref<TypeSpecifier> FieldExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *currentScope = table.CurrentScope();

        SymbolNode *fieldNode = currentScope->Iter().StructField(m_Name.Id()).Find();
        if (!fieldNode)
        {
            throw "Cannot find the struct field named {}";
        }
        Ref<TypeSpecifier> typeSpecifier = m_Value->Analyze(semanticAnalyzer);

        VariableSymbolNode *variableNode = fieldNode->Into<VariableSymbolNode>();

        if (*typeSpecifier != *variableNode->GetTypeSpecifier())
        {
            throw "Struct type and expression types do not matches";
        }

        return typeSpecifier;
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
