#pragma once
#include "Ast/Program.hpp"
#include "Ast/Definitions.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{

    class SemanticAnalyzer
    {
    public:
        SemanticAnalyzer(Ref<Program> program);
        ~SemanticAnalyzer() = default;

        void Analyze();
        const std::string &InstantiateGenerics(const std::string &name, const Generics *generics);
        const std::string &InstantiateGenerics(const std::string &name, const std::vector<Ref<TypeSpecifier>> &typeArgs);

    private:
        void AddExpandedDefinition(Box<Definition> definition);
        void GenerateGenericKey(GenericInstanceKey &key, const Definition *definition, const std::vector<Ref<TypeSpecifier>> &typeArgs);
        Box<Definition> Instantiate(SymbolNode *node, GenericInstanceKey &key, const std::vector<Ref<TypeSpecifier>> &typeArgs);

        Ref<Program> m_Program;
        std::unordered_map<GenericInstanceKey, std::string, GenericInstanceKeyHasher> m_Generis;
    };

} // namespace Marble
