#pragma once

#include <unordered_set>

#include "Ast/Program.hpp"
#include "Ast/Definitions.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "SemanticAnalyzer/Conversion.hpp"

namespace Marble
{
    class File;

    struct MemberFunctionInstanceKey
    {
        std::string MethodName;
        std::string StructName;
        std::vector<std::string> Params;

        bool operator==(const MemberFunctionInstanceKey &obj)
        {
            if (obj.Params.size() != Params.size())
            {
                return false;
            }

            for (size_t i = 0; i < obj.Params.size(); i++)
            {
                if (obj.Params[i] != Params[i])
                {
                    return false;
                }
            }
            return obj.MethodName == MethodName && obj.StructName == StructName;
        }
    };

    struct MemberFunctionInstanceKeyHasher
    {
        std::size_t operator()(const MemberFunctionInstanceKey &key) const
        {
            size_t result = std::hash<std::string>()(key.MethodName);
            result ^= std::hash<std::string>()(key.StructName) + 0x9e3779b9 + (result << 6) + (result >> 2);

            for (const auto &param : key.Params)
            {
                result ^= std::hash<std::string>()(param) + 0x9e3779b9 + (result << 6) + (result >> 2);
            }

            return result;
        }
    };

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
        void RegisterMethod(const std::string &implName, const MemberFunctionDefinition &memberFunctionDefinition, const std::string &functionName);

        const std::string &InstantiateGenerics(const std::string &name, const Generics *generics);
        const std::string &InstantiateGenerics(const std::string &name, const std::vector<Ref<TypeSpecifier>> &typeArgs);
        const std::string &ConvertMethodIntoFunction(Definition *definition, const std::string &structName, SymbolNode *currectScope);
        void AddExpandedDefinition(Box<Definition> definition);

    private:
        void GenerateGenericKey(GenericInstanceKey &key, const Definition *definition, const std::vector<Ref<TypeSpecifier>> &typeArgs);
        void ExpandNestedGenerics(const std::vector<Ref<TypeSpecifier>> &typeArgs);
        Box<Definition> Instantiate(SymbolNode *node, GenericInstanceKey &key, const std::vector<Ref<TypeSpecifier>> &typeArgs);
        Box<Identifier> CreateExpandedName(const Identifier &Id, const std::vector<Ref<TypeSpecifier>> &typeArgs);

    private:
        const Marble::File &m_File;
        Conversion m_Conversion;
        Ref<Program> m_Program;
        std::unordered_map<GenericInstanceKey, std::string, GenericInstanceKeyHasher> m_Generis;
        std::unordered_map<MemberFunctionInstanceKey, std::string, MemberFunctionInstanceKeyHasher> m_ConvertedMemberFunctionsName;
    };

} // namespace Marble
