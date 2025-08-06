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
        SymbolNode *node = SymbolIterator().StructOrEnum(GetName());
        if (!node)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find related struct or enum", true);
        }
        if (m_Generics)
        {
            TODO("HANDLE IMPL GENERICS");
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

        Box<Identifier> functionName = MakeBox<Identifier>(node->GetSymbolData().Name() + "_" + m_Prototype->GetName() + "_" + IDGenerator::Generate(), Span{});

        size_t paramSize = m_Prototype->m_Params.size();
        std::vector<Box<VariableType>> params;
        params.reserve(paramSize);
        if (m_Prototype->m_Method)
        {
            params.push_back(std::move(m_Prototype->m_Method));
        }
        for (auto &param : m_Prototype->m_Params)
        {
            params.push_back(std::move(param));
        }

        StructOrEnumSymbolNode *concreteNode = node->Into<StructOrEnumSymbolNode>();
        concreteNode->InsertMethod(m_Prototype->m_Name->Id(), functionName->Id());

        AccessSpecifier accessSpecifier = m_Prototype->GetAccessSpecifier();
        Box<Generics> generics = std::move(m_Prototype->m_Generics);
        Ref<TypeSpecifier> returnType = m_Prototype->m_ReturnType;
        Box<Statement> block = std::move(m_Block);
        Box<FunctionDefinition> fnDefinition =
            MakeBox<FunctionDefinition>(accessSpecifier, std::move(functionName), std::move(generics), std::move(params), returnType, std::move(block), m_Span);

        SymbolTable::Get().Insert(*fnDefinition, true);
        semanticAnalyzer.RegisterFunction(std::move(fnDefinition));

        return TypeSpecifierOk;
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
