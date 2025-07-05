#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "SemanticAnalyzer.hpp"
#include "Utils/Macros.hpp"

namespace Marble
{
    SemanticAnalyzer::SemanticAnalyzer(Ref<Program> program) : m_Program{program}
    {
    }

    void SemanticAnalyzer::Analyze()
    {
        for (size_t i = 0; i < m_Program->Definitions().size(); i++)
        {
            auto &definition = m_Program->Definitions()[i];
            if (!definition->IsGeneric() && !definition->IsAnalyzed())
            {
                definition->Analyze(*this);
            }
        }
    }

    const std::string &SemanticAnalyzer::InstantiateGenerics(const std::string &name, const Generics *generics)
    {
        if (!generics)
        {
            throw "Generic arguments must be provided";
        }
        return InstantiateGenerics(name, generics->Types());
    }

    const std::string &SemanticAnalyzer::InstantiateGenerics(const std::string &name, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *scope = table.CurrentScope();
        SymbolNode *parent = scope->Iter().Parent().Find();

        GenericInstanceKey key;

        if (auto node = parent->Iter().Function(name).Find(); node)
        {
            Box<Definition> expandedFnDefinition = Instantiate(node, key, typeArgs);
            if (!expandedFnDefinition)
            {
                return m_Generis[key];
            }

            FunctionSymbolNode *newFnSymbol = new FunctionSymbolNode{*expandedFnDefinition->Into<FunctionDefinition>(),
                                                                     parent};
            parent->Insert(expandedFnDefinition->GetName(), newFnSymbol);
            AddExpandedDefinition(std::move(expandedFnDefinition));
            return m_Generis[key];
        }

        if (auto node = table.Root()->Iter().Struct(name).Find(); node)
        {
            Box<Definition> expandedStructDefinition = Instantiate(node, key, typeArgs);
            if (!expandedStructDefinition)
            {
                return m_Generis[key];
            }
            StructDefinition *newStructDefinition = expandedStructDefinition->Into<StructDefinition>();
            SymbolNode *newStructNode = new SymbolNode{*newStructDefinition, table.Root()};
            table.Insert(expandedStructDefinition->GetName(), newStructNode);

            ImplDefinition *implDefinition = newStructDefinition->GetImplDefinition();
            if (!implDefinition)
            {
                return m_Generis[key];
            }
            Box<Definition> expandedImplDefinition = implDefinition->InstantiateWith(*this, typeArgs);
            ImplDefinition *newImplDefinition = expandedImplDefinition->Into<ImplDefinition>();

            Ref<TypeSpecifier> implName = MakeRef<TypeSpecifier>(
                Identifier{newStructDefinition->GetName(), newStructDefinition->GetStructName().GetSpan()},
                newStructDefinition->GetStructName().GetSpan());

            newImplDefinition->SetImplName(implName);
            newImplDefinition->CreateSymbol();
            AddExpandedDefinition(std::move(expandedStructDefinition));
            AddExpandedDefinition(std::move(expandedImplDefinition));
            return m_Generis[key];
        }

        if (auto node = scope->Iter().Function(name).Find(); node)
        {
            Box<Definition> expandedMethodDefinition = Instantiate(node, key, typeArgs);
            if (!expandedMethodDefinition)
            {
                return m_Generis[key];
            }
            MemberFunctionDefinition *newMemberFunctionDefinition = expandedMethodDefinition->Into<MemberFunctionDefinition>();
            FunctionSymbolNode *newFunctionNode = new FunctionSymbolNode{*newMemberFunctionDefinition, scope};
            scope->Insert(newMemberFunctionDefinition->GetName(), newFunctionNode);

            expandedMethodDefinition->Analyze(*this);

            Definition *definition = static_cast<Definition *>(scope->GetAstPtr());
            StructDefinition *structDefinition = definition->Into<StructDefinition>();
            ImplDefinition *implDefinition = structDefinition->GetImplDefinition();
            implDefinition->AddMemberFunction(Box<MemberFunctionDefinition>(expandedMethodDefinition.release()->Into<MemberFunctionDefinition>()));
            return m_Generis[key];
        }
        ASSERT_D(false, "SOMETHING WENT WRONG");
        UNREACHABLE();
    }

    Box<Definition> SemanticAnalyzer::Instantiate(SymbolNode *node, GenericInstanceKey &key, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {

        Definition *definition = static_cast<Definition *>(node->GetAstPtr());
        GenerateGenericKey(key, definition, typeArgs);
        if (m_Generis.contains(key))
        {
            return nullptr;
        }
        Box<Definition> expandedDefinition = definition->InstantiateWith(*this, typeArgs);
        m_Generis[key] = expandedDefinition->GetName();
        return expandedDefinition;
    }

    void SemanticAnalyzer::AddExpandedDefinition(Box<Definition> definition)
    {
        auto &definitions = m_Program->Definitions();
        for (auto &def : definitions)
        {
            if (def->GetName() == definition->GetName() && definition->DefinitionType() == def->DefinitionType())
            {
                return;
            }
        }
        definitions.push_back(std::move(definition));
    }

    void SemanticAnalyzer::GenerateGenericKey(GenericInstanceKey &key, const Definition *definition, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        key.Name = definition->GetName();
        key.KeyType = GenericInstanceKey::FromDefinitionType((int)definition->DefinitionType());
        for (auto &type : typeArgs)
        {
            key.TypeArgumentNames.emplace_back(type->ToString());
        }
    }

} // namespace Marble
