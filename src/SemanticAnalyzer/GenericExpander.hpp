#pragma once

#include <unordered_map>
#include "Ast/Program.hpp"
#include "Ast/Generics.hpp"

namespace Marble
{
    class GenericExpander
    {
    public:
        using TypeArgs = std::vector<Ref<TypeSpecifier>>;

        GenericExpander(SemanticAnalyzer &semanticAnalyzer);
        ~GenericExpander() = default;

        const std::string &Expand(const std::string &name, const TypeArgs &typeArgs);

    private:
        void HandleNested(const TypeArgs &typeArgs);
        void GenerateKey(GenericInstanceKey &key, const Definition *definition, const TypeArgs &typeArgs);
        Box<Identifier> GenerateConcreteName(const Identifier &id, const TypeArgs &typeArgs);
        Box<Definition> Instantiate(Definition *definition, const GenericInstanceKey &key, const TypeArgs &typeArgs);
        void InsertDefinition(Box<Definition> definition);

    private:
        SemanticAnalyzer &m_SemanticAnalyzer;
        std::unordered_map<GenericInstanceKey, std::string, GenericInstanceKeyHasher> m_Generis;
    };

} // namespace Marble
