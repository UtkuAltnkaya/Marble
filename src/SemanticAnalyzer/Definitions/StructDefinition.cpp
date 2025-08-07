#include "Ast/Definitions.hpp"
#include "Utils/IDGenerator.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    Ref<TypeSpecifier> StructDefinition::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        for (auto &field : m_Field)
        {
            auto castedField = field->Into<StructFieldDefinition>();
            Ref<TypeSpecifier> paramType = castedField->GetField().GetTypeSpecifier();
            if (paramType->IsPrimitive())
            {
                continue;
            }
            CheckParametersType(semanticAnalyzer, paramType);
        }
        m_IsAnalyzed = true;
        return TypeSpecifierOk;
    }

    Box<Definition> StructDefinition::InstantiateWith(GenericExpander &genericExpander, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        Box<Definition> clonedStructDef = Clone();
        StructDefinition *castedStruct = clonedStructDef->Into<StructDefinition>();
        auto map = m_Generics->ToMap(typeArgs);
        castedStruct->SubstituteGenerics(genericExpander, map);
        castedStruct->m_Generics.reset();
        m_IsExpanded = true;
        return clonedStructDef;
    }

    void StructDefinition::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        for (auto &field : m_Field)
        {
            field->GetField().GetTypeSpecifier()->SubstituteGenerics(genericExpander, map);
        }
    }

    Box<Definition> StructDefinition::Clone()
    {
        return MakeBox<StructDefinition>(*this);
    }

    Box<Definition> StructFieldDefinition::Clone()
    {
        return MakeBox<StructFieldDefinition>(*this);
    }
} // namespace Marble
