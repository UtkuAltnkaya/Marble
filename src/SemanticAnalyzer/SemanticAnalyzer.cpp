#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "SemanticAnalyzer.hpp"

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
            std::cout << definition->GetName() << std::endl;
            if (!definition->IsGeneric() && !definition->IsAnalyzed())
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

        Box<Definition> newDefinition = definition->InstantiateWith(generics->Types());
        Definition *concreate = newDefinition.release();
        m_Generis[key] = concreate;
        return concreate;
    }

    void SemanticAnalyzer::AddExpandedDefinition(Definition *definition)
    {
        auto &defs = m_Program->Definitions();
        for (auto &def : defs)
        {
            if (def->GetName() == definition->GetName())
            {
                return;
            }
        }
        defs.emplace_back(definition);
    }

    GenericDefinition *SemanticAnalyzer::Find(const std::string &name)
    {
        for (auto &def : m_Program->Definitions())
        {
            if (def->GetName() == name && def->IsGeneric())
            {
                auto gDef = dynamic_cast<GenericDefinition *>(def.get());
                ASSERT_D(gDef != nullptr, "Cast failed");
                return gDef;
            }
        }
        return nullptr;
    }

} // namespace Marble
