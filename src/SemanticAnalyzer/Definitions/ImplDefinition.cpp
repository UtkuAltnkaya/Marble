#include "Ast/Definitions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include <iostream>
#include "Utils/IDGenerator.hpp"

namespace Marble
{

    Ref<TypeSpecifier> ImplDefinition::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *node = table.Root();

        if (!m_ImplName->IsPrimitive())
        {
            node = HandleRoot(node);
        }

        table.EnterScope(node);
        for (auto &memberFunction : m_MemberFunctions)
        {
            if (!memberFunction->IsGeneric() && !memberFunction->IsAnalyzed())
            {
                memberFunction->Analyze(semanticAnalyzer);
            }
        }
        table.LeaveScope();

        return TypeSpecifierOk;
    }

    Ref<TypeSpecifier> MemberFunctionDefinition::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        const SymbolNode *node = table.CurrentScope();
        SymbolNode *fnNode = node->Iter()
                                 .Function(m_Prototype->GetName())
                                 .Find();
        if (!fnNode)
        {
            throw "Cannot find function in this scope";
        }

        table.EnterScope(fnNode);
        m_Block->Analyze(semanticAnalyzer);
        table.LeaveScope();

        if (m_Prototype->GetReturnType()->GetType() == Types::Void)
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

    Box<Definition> ImplDefinition::InstantiateWith(SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        Box<Definition> clonedImplDef = Clone();
        ImplDefinition *castedImplDef = clonedImplDef->Into<ImplDefinition>();
        auto map = m_Generics->ToMap(typeArgs);
        castedImplDef->SubstituteGenerics(semanticAnalyzer, map);
        castedImplDef->m_Generics.reset();
        m_IsExpanded = true;
        return clonedImplDef;
    }

    void ImplDefinition::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        for (auto &memberFunction : m_MemberFunctions)
        {
            memberFunction->SubstituteGenerics(semanticAnalyzer, map);
        }
    }

    void ImplDefinition::CreateSymbol()
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *node = table.Root();

        if (!m_ImplName->IsPrimitive())
        {
            node = HandleRoot(node);
        }

        for (auto &memberFunction : m_MemberFunctions)
        {
            node->Insert(memberFunction->GetPrototype().GetName(), new FunctionSymbolNode{*memberFunction.get(), node});
        }
    }

    Box<Definition> MemberFunctionDefinition::InstantiateWith(SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        Box<MemberFunctionDefinition> clonedMemberFn = MakeBox<MemberFunctionDefinition>(*this);
        auto map = m_Prototype->GetGenerics()->ToMap(typeArgs);
        clonedMemberFn->SubstituteGenerics(semanticAnalyzer, map);

        std::string name = clonedMemberFn->m_Prototype->GetName();
        for (auto &type : typeArgs)
        {
            name += "_" + type->ToString();
        }
        name += "_" + IDGenerator::Generate();
        clonedMemberFn->m_Prototype->m_Name = MakeBox<Identifier>(name, clonedMemberFn->m_Prototype->m_Name->GetSpan());
        clonedMemberFn->m_Prototype->m_Generics.reset();
        m_IsExpanded = true;
        m_Prototype->m_IsExpanded = true;
        return Box<Definition>(clonedMemberFn.release());
    }

    void MemberFunctionDefinition::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Prototype->SubstituteGenerics(semanticAnalyzer, map);
        m_Block->SubstituteGenerics(semanticAnalyzer, map);
    }

    Box<Definition> MemberFunctionPrototypeDefinition::InstantiateWith(SemanticAnalyzer &semanticAnalyzer, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        Box<MemberFunctionPrototypeDefinition> clonedMemberFnPrototype = MakeBox<MemberFunctionPrototypeDefinition>(*this);
        auto map = m_Generics->ToMap(typeArgs);
        clonedMemberFnPrototype->SubstituteGenerics(semanticAnalyzer, map);

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

    void MemberFunctionPrototypeDefinition::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_Method)
        {
            m_Method->GetTypeSpecifier()->SubstituteGenerics(semanticAnalyzer, map);
        }
        for (auto &param : m_Params)
        {
            param->GetTypeSpecifier()->SubstituteGenerics(semanticAnalyzer, map);
        }
        m_ReturnType->SubstituteGenerics(semanticAnalyzer, map);
    }

    SymbolNode *ImplDefinition::HandleRoot(SymbolNode *node)
    {
        SymbolIterator iter = node->Iter();
        const std::string &name = m_ImplName->UserDefine().Id();

        if (auto symbolNode = iter.Struct(name).Find(); symbolNode)
        {
            auto structNode = static_cast<StructDefinition *>(symbolNode->GetAstPtr());
            if (structNode && !structNode->GetImplDefinition())
            {
                structNode->SetImplDefinition(this);
            }
            return symbolNode;
        }
        if (auto symbolNode = iter.Reset().Enum(name).Find(); symbolNode)
        {
            return symbolNode;
        }
        throw "No such struct or enum";
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
