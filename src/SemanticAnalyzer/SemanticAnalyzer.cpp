#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "SemanticAnalyzer.hpp"

namespace Marble
{
    SemanticAnalyzer::SemanticAnalyzer(Ref<Program> program) : m_Program{program}
    {
    }

    void SemanticAnalyzer::Analyze()
    {
        std::vector<Definition *> genericsFunctions;

        for (auto &definition : m_Program->Definitions())
        {
            if (!definition->IsGeneric())
            {
                definition->Analyze(*this);
            }
        }
    }

    Definition *SemanticAnalyzer::InstantiateGenerics(const std::string &name, Generics *generics)
    {
        GenericInstanceKey key;
        GenericDefinition *definition = Find(name);
        key.Name = definition->GetName();
        for (auto &type : generics->Types())
        {
            key.TypeArgumentNames.emplace_back(type->ToString());
        }
        if (m_Generis.contains(key))
        {
            return m_Generis[key];
        }

        definition->InstantiateWith(generics->Types());
    }

    GenericDefinition *SemanticAnalyzer::Find(const std::string &name)
    {
        for (auto &def : m_Program->Definitions())
        {
            if (def->GetName() == name && def->IsGeneric())
            {
                return (GenericDefinition *)def.get();
            }
        }
    }

} // namespace Marble
