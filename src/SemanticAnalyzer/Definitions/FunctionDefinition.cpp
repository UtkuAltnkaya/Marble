#include <iostream>
#include "ErrorSystem/ErrorSystem.hpp"
#include "Ast/Definitions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "Utils/IDGenerator.hpp"

namespace Marble
{
    Ref<TypeSpecifier> FunctionDefinition::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        m_IsAnalyzed = true;
        FunctionSymbolNode *fnNode = SymbolIterator().Function(m_FunctionName->Id());
        if (!fnNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find function in this scope");
        }

        BlockSymbolNode *blockNode = fnNode->Block();
        SymbolScopeGuard guard{blockNode};

        for (auto &param : m_Params)
        {
            Ref<TypeSpecifier> paramType = param->GetTypeSpecifier();
            if (paramType->IsPrimitive())
            {
                continue;
            }
            CheckParametersType(semanticAnalyzer, paramType);
        }

        m_Block->Analyze(semanticAnalyzer);

        if (m_ReturnType->GetType() == Types::Void)
        {
            return TypeSpecifierOk;
        }

        BlockStatement *block = m_Block->Into<BlockStatement>();

        const std::vector<Box<Statement>> &statements = block->Statements();
        if (statements.size() == 0)
        {
            ErrorSystem::AddWarn(semanticAnalyzer, this, "Empty function body");
            return TypeSpecifierOk;
        }

        bool returnFlag = false;
        for (size_t i = 0; i < statements.size(); i++)
        {
            if (returnFlag)
            {
                ErrorSystem::AddWarn(semanticAnalyzer, statements[i].get(), "Dead code");
            }

            if (statements[i]->StatementType() == StatementType::Return)
            {
                returnFlag = true;
            }
        }
        if (returnFlag)
        {
            return TypeSpecifierOk;
        }
        ErrorSystem::AddError(semanticAnalyzer, this, "Return statement expected");
        return TypeSpecifierOk;
    }

    Box<Definition> FunctionDefinition::InstantiateWith(SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        Box<Definition> clonedFnDef = Clone();
        FunctionDefinition *castFnDef = clonedFnDef->Into<FunctionDefinition>();
        auto map = m_Generics->ToMap(typeArgs);
        castFnDef->SubstituteGenerics(semanticAnalyzer, map);
        castFnDef->m_Generics.reset();
        m_IsExpanded = true;
        return clonedFnDef;
    }

    void FunctionDefinition::SubstituteGenerics(
        SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        for (auto &param : m_Params)
        {
            param->GetTypeSpecifier()->SubstituteGenerics(semanticAnalyzer, map);
        }
        m_ReturnType->SubstituteGenerics(semanticAnalyzer, map);
        m_Block->SubstituteGenerics(semanticAnalyzer, map);
    }

    bool FunctionDefinition::operator==(const FunctionDefinition &obj) const
    {
        if (m_AccessSpecifier != obj.m_AccessSpecifier)
        {
            return false;
        }
        if (*m_FunctionName != *obj.m_FunctionName)
        {
            return false;
        }
        if (m_Generics && obj.m_Generics && *m_Generics != *obj.m_Generics)
        {
            return false;
        }
        if (m_Params.size() != obj.m_Params.size())
        {
            return false;
        }
        for (size_t i = 0; i < m_Params.size(); i++)
        {
            if (*m_Params[i] != *obj.m_Params[i])
            {
                return false;
            }
        }
        if (*m_ReturnType != *obj.m_ReturnType)
        {
            return false;
        }
        // TODO compare block
        return true;
    }

    Box<Definition> FunctionDefinition::Clone()
    {
        return MakeBox<FunctionDefinition>(*this);
    }
} // namespace Marble
