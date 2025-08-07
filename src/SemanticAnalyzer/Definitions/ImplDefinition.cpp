#include <iostream>
#include "Ast/Definitions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "Utils/IDGenerator.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{

    Ref<TypeSpecifier> ImplDefinition::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        m_IsAnalyzed = true;
        StructOrEnumSymbolNode *node = SymbolIterator().StructOrEnum(GetName());
        if (!node)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find related struct or enum", true);
        }
        SymbolScopeGuard guard{node};
        for (auto &memberFunction : m_MemberFunctions)
        {
            memberFunction->Analyze(semanticAnalyzer);
        }
        return TypeSpecifierOk;
    }

    Ref<TypeSpecifier> MemberFunctionDefinition::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolNode *node = SymbolTable::Get().CurrentScope();
        Box<FunctionDefinition> fnDefinition = MakeBox<FunctionDefinition>(std::move(*this), node->GetSymbolData().Name());
        StructOrEnumSymbolNode *concreteNode = node->Into<StructOrEnumSymbolNode>();

        concreteNode->InsertMethod(m_Prototype->m_Name->Id(), fnDefinition->GetName());
        SymbolTable::Get().Insert(*fnDefinition);
        semanticAnalyzer.RegisterFunction(std::move(fnDefinition));
        return TypeSpecifierOk;
    }

    Box<Definition> ImplDefinition::InstantiateWith(GenericExpander &genericExpander, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        Box<Definition> clonedImplDefinition = Clone();
        auto map = m_Generics->ToMap(typeArgs);
        clonedImplDefinition->SubstituteGenerics(genericExpander, map);
        clonedImplDefinition->Into<ImplDefinition>()->m_Generics.reset();
        m_IsExpanded = true;
        return clonedImplDefinition;
    }

    void ImplDefinition::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        for (auto &memberFunction : m_MemberFunctions)
        {
            memberFunction->SubstituteGenerics(genericExpander, map);
        }
    }

    Box<Definition> MemberFunctionDefinition::InstantiateWith(GenericExpander &genericExpander, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        Box<MemberFunctionDefinition> clonedMemberFn = MakeBox<MemberFunctionDefinition>(*this);
        auto map = m_Prototype->GetGenerics()->ToMap(typeArgs);
        clonedMemberFn->SubstituteGenerics(genericExpander, map);
        clonedMemberFn->m_Prototype->m_Generics.reset();
        m_IsExpanded = true;
        m_Prototype->m_IsExpanded = true;
        return Box<Definition>(clonedMemberFn.release());
    }

    void MemberFunctionDefinition::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Prototype->SubstituteGenerics(genericExpander, map);
        m_Block->SubstituteGenerics(genericExpander, map);
    }

    Box<Definition> MemberFunctionPrototypeDefinition::InstantiateWith(GenericExpander &genericExpander, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        Box<MemberFunctionPrototypeDefinition> clonedMemberFnPrototype = MakeBox<MemberFunctionPrototypeDefinition>(*this);
        auto map = m_Generics->ToMap(typeArgs);
        clonedMemberFnPrototype->SubstituteGenerics(genericExpander, map);

        std::string name = clonedMemberFnPrototype->GetName();
        for (auto &type : typeArgs)
        {
            name += "_" + type->ToString();
        }
        name += "_" + IDGenerator::Generate();
        clonedMemberFnPrototype->m_Name = MakeBox<Identifier>(name, clonedMemberFnPrototype->m_Name->GetSpan());
        clonedMemberFnPrototype->m_Generics.reset();
        m_IsExpanded = true;
        return Box<Definition>(clonedMemberFnPrototype.release());
    }

    void MemberFunctionPrototypeDefinition::SubstituteGenerics(GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_Method)
        {
            m_Method->GetTypeSpecifier()->SubstituteGenerics(genericExpander, map);
        }
        for (auto &param : m_Params)
        {
            param->GetTypeSpecifier()->SubstituteGenerics(genericExpander, map);
        }
        m_ReturnType->SubstituteGenerics(genericExpander, map);
    }

    Box<Definition> ImplDefinition::Clone()
    {
        return MakeBox<ImplDefinition>(*this);
    }

    Box<Definition> MemberFunctionDefinition::Clone()
    {
        return MakeBox<MemberFunctionDefinition>(*this);
    }

    Box<Definition> MemberFunctionPrototypeDefinition::Clone()
    {
        return MakeBox<MemberFunctionPrototypeDefinition>(*this);
    }
} // namespace Marble
