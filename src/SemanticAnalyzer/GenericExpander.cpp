#include "SemanticAnalyzer/GenericExpander.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "Utils/IDGenerator.hpp"

namespace Marble
{
    GenericExpander::GenericExpander(SemanticAnalyzer &semanticAnalyzer) : m_SemanticAnalyzer{semanticAnalyzer}
    {
    }

    const std::string &GenericExpander::Expand(const std::string &name, const TypeArgs &typeArgs)
    {
        HandleNested(typeArgs);

        Definition *definition = nullptr;
        ImplDefinition *implDefinition = nullptr;
        GenericInstanceKey key;

        if (auto functionNode = SymbolIterator().Function(name); functionNode)
        {
            definition = functionNode->Ast();
            GenerateKey(key, definition, typeArgs);
        }
        else if (auto structNode = SymbolIterator().Struct(name); structNode)
        {
            StructDefinition *structDefinition = structNode->Ast<StructDefinition>();
            GenerateKey(key, structDefinition, typeArgs);
            implDefinition = structDefinition->GetImplDefinition();
            definition = structDefinition;
        }
        if (m_Generis.contains(key))
        {
            return m_Generis[key];
        }
        Box<Definition> expandedDefinition = Instantiate(definition, key, typeArgs);

        SymbolTable::Get().Insert(expandedDefinition.get());
        if (implDefinition)
        {
            Box<Definition> expandedImplDefinition = implDefinition->InstantiateWith(*this, typeArgs);
            ImplDefinition *castedImplDefinition = expandedImplDefinition->Into<ImplDefinition>();
            Ref<TypeSpecifier> implName = MakeRef<TypeSpecifier>(
                Identifier{expandedDefinition->GetName(), expandedDefinition->GetIdentifier().GetSpan()},
                definition->GetIdentifier().GetSpan());
            castedImplDefinition->SetImplName(implName);
            castedImplDefinition->Analyze(m_SemanticAnalyzer);
        }
        InsertDefinition(std::move(expandedDefinition));
        return m_Generis[key];
    }

    void GenericExpander::HandleNested(const TypeArgs &typeArgs)
    {
        auto &vector = const_cast<std::vector<Ref<TypeSpecifier>> &>(typeArgs);

        for (size_t i = 0; i < vector.size(); i++)
        {
            auto &typeSpecifier = vector[i];
            if (typeSpecifier->GetType() != Types::GenericType)
            {
                continue;
            }
            const GenericType &generics = typeSpecifier->GenericUnchecked();
            const std::string &name = Expand(*generics.OuterType, generics.InnerType);

            Span span;
            span.Start = generics.OuterType.GetSpan().Start;
            span.End = Position{span.Start.Row, span.Start.Col + name.size(), span.Start.Cursor + name.size()};

            Ref<TypeSpecifier> expandedTypeSpecifier = MakeRef<TypeSpecifier>(Identifier{name, span}, span);
            vector[i].swap(expandedTypeSpecifier);
        }
    }

    void GenericExpander::GenerateKey(GenericInstanceKey &key, const Definition *definition, const TypeArgs &typeArgs)
    {
        key.Name = definition->GetName();
        key.KeyType = GenericInstanceKey::FromDefinitionType((int)definition->DefinitionType());
        for (auto &type : typeArgs)
        {
            key.TypeArgumentNames.emplace_back(type->ToString());
        }
    }

    Box<Definition> GenericExpander::Instantiate(Definition *definition, const GenericInstanceKey &key, const TypeArgs &typeArgs)
    {
        Box<Identifier> id = GenerateConcreteName(definition->GetIdentifier(), typeArgs);
        m_Generis[key] = id->Id();
        Box<Definition> expandendDefinition = definition->InstantiateWith(*this, typeArgs);
        expandendDefinition->SetName(std::move(id));
        return expandendDefinition;
    }

    Box<Identifier> GenericExpander::GenerateConcreteName(const Identifier &id, const TypeArgs &typeArgs)
    {
        std::string name = *id;
        for (auto &type : typeArgs)
        {
            name += "_" + type->ToString();
        }
        name += "_" + IDGenerator::Generate();
        return MakeBox<Identifier>(name, id.GetSpan());
    }

    void GenericExpander::InsertDefinition(Box<Definition> definition)
    {
        auto &definitions = m_SemanticAnalyzer.Program()->Definitions();

        for (auto &def : definitions)
        {
            if (def->GetName() == definition->GetName() && def->DefinitionType() == definition->DefinitionType())
            {
                return;
            }
        }
        definitions.push_back(std::move(definition));
    }
} // namespace Marble
