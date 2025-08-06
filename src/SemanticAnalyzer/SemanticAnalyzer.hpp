#pragma once

#include "Ast/Program.hpp"
#include "Ast/Definitions.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "SemanticAnalyzer/Conversion.hpp"
#include "SemanticAnalyzer/GenericExpander.hpp"

namespace Marble
{
    class File;

    class SemanticAnalyzer
    {
    public:
        SemanticAnalyzer(Ref<Program> program, const Marble::File &file);
        ~SemanticAnalyzer() = default;

        void Analyze();
        bool TryImplicitConversion(Box<Expression> &expr, Ref<TypeSpecifier> from, Ref<TypeSpecifier> to);
        ConversionKind CanConvert(Ref<TypeSpecifier> from, Ref<TypeSpecifier> to);
        Ref<TypeSpecifier> UnifyArithmeticTypes(Ref<TypeSpecifier> a, Ref<TypeSpecifier> b);
        inline const Marble::File &File() const { return m_File; }
        void RegisterFunction(Box<FunctionDefinition> functionDefinition);

        const std::string &InstantiateGenerics(const std::string &name, const Generics *generics);
        const std::string &InstantiateGenerics(const std::string &name, const std::vector<Ref<TypeSpecifier>> &typeArgs);

    private:
        Ref<Program> m_Program;
        const Marble::File &m_File;
        Conversion m_Conversion;
        GenericExpander m_GenericExpander;
    };

} // namespace Marble
