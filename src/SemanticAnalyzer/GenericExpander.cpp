#include "SemanticAnalyzer/GenericExpander.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "Utils/IDGenerator.hpp"

namespace Marble
{
    GenericExpander::GenericExpander(Ref<Program> program) : m_Program{program}
    {
    }

    const std::string &GenericExpander::Expand(const std::string &name, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        GenericInstanceKey key;
        if (auto functionNode = SymbolIterator().Function(name); functionNode)
        {
            FunctionDefinition *functionDefinition = functionNode->Ast();
            key.Name = functionDefinition->GetName();
            key.KeyType = GenericInstanceKey::FromDefinitionType((int)functionDefinition->DefinitionType());
            for (auto &type : typeArgs)
            {
                key.TypeArgumentNames.emplace_back(type->ToString());
            }
            if (m_Generis.contains(key))
            {
                return m_Generis[key];
            }

            Box<Identifier> id = GenerateConcreteName(functionDefinition->GetIdentifier(), typeArgs);
            m_Generis[key] = id->Id();

            // Box<Definition> expandendDefinition = functionDefinition->InstantiateWith()
        }
    }

    Box<Identifier> GenericExpander::GenerateConcreteName(const Identifier &id, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        std::string name = *id;
        for (auto &type : typeArgs)
        {
            name += "_" + type->ToString();
        }
        name += "_" + IDGenerator::Generate();
        return MakeBox<Identifier>(name, id.GetSpan());
    }

} // namespace Marble
