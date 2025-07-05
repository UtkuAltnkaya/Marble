#include "Ast/Definitions.hpp"
#include <iostream>
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "Utils/IDGenerator.hpp"

namespace Marble
{

    Ref<TypeSpecifier> FunctionDefinition::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        m_IsAnalyzed = true;
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *iter = table.Iter()
                               .Function(m_FunctionName->Id())
                               .Find();
        if (!iter)
        {
            throw "Cannot find function in this scope";
        }

        table.EnterScope(iter);

        m_Block->Analyze(semanticAnalyzer);
        table.LeaveScope();

        if (m_ReturnType->GetType() == Types::Void)
        {
            return TypeSpecifierOk;
        }

        BlockStatement *block = static_cast<BlockStatement *>(m_Block.get());

        const std::vector<Box<Statement>> &statements = block->Statements();
        if (statements.size() == 0)
        {
            // TODO: Decide to allow empty function body or not, or warn
        }

        if (statements.back()->StatementType() == StatementType::Return)
        {
            return TypeSpecifierOk;
        }
        throw "Return statement expected";
    }

    Box<Definition> FunctionDefinition::InstantiateWith(SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        Box<Definition> clonedFnDef = Clone();
        FunctionDefinition *castFnDef = clonedFnDef->Into<FunctionDefinition>();
        auto map = m_Generics->ToMap(typeArgs);
        castFnDef->SubstituteGenerics(semanticAnalyzer, map);

        std::string name = castFnDef->GetName();
        for (auto &type : typeArgs)
        {
            name += "_" + type->ToString();
        }
        name += "_" + IDGenerator::Generate();
        castFnDef->m_FunctionName = MakeBox<Identifier>(name, castFnDef->m_FunctionName->GetSpan());
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
