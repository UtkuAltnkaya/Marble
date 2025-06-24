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
        Definition *InstantiateGenerics(const std::string &name, Generics *generics);
        void AddExpandedDefinition(Definition *definition);

    private:
        GenericDefinition *Find(const std::string &name);

        Ref<Program> m_Program;
        std::unordered_map<GenericInstanceKey, Definition *, GenericInstanceKeyHasher> m_Generis;
    };

} // namespace Marble
