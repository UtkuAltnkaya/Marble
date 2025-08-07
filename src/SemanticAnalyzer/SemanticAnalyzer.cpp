#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"
#include "Utils/Macros.hpp"
#include "Utils/IDGenerator.hpp"
#include "Utils/File.hpp"

namespace Marble
{
    static int RankTypes(Types t);

    SemanticAnalyzer::SemanticAnalyzer(Ref<Marble::Program> program, const Marble::File &file)
        : m_Program{program}, m_File{file}, m_GenericExpander{*this}
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

        auto &vec = m_Program->Definitions();
        vec.erase(std::remove_if(vec.begin(), vec.end(), [](Box<Definition> &definition)
                                 { return definition->IsGeneric() || definition->DefinitionType() == DefinitionType::Impl; }),
                  vec.end());
    }

    void SemanticAnalyzer::RegisterFunction(Box<FunctionDefinition> functionDefinition)
    {
        m_Program->Definitions().push_back(std::move(functionDefinition));
    }

    const std::string &SemanticAnalyzer::InstantiateGenerics(const std::string &name, const Generics *generics)
    {
        ASSERT_A(generics != nullptr, "Generic must be provided");
        return InstantiateGenerics(name, generics->Types());
    }

    const std::string &SemanticAnalyzer::InstantiateGenerics(const std::string &name, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        return m_GenericExpander.Expand(name, typeArgs);
    }

    bool SemanticAnalyzer::TryImplicitConversion(Box<Expression> &expr, Ref<TypeSpecifier> from, Ref<TypeSpecifier> to)
    {
        ConversionKind conversionKind = m_Conversion.CanConvert(from, to);
        if (conversionKind == ConversionKind::None)
        {
            return false;
        }
        if (conversionKind == ConversionKind::Identity)
        {
            return true;
        }
        Span span = expr->GetSpan();
        expr = MakeBox<CastExpression>(to, std::move(expr), conversionKind, span);
        return true;
    }

    ConversionKind SemanticAnalyzer::CanConvert(Ref<TypeSpecifier> from, Ref<TypeSpecifier> to)
    {
        return m_Conversion.CanConvert(from, to);
    }

    Ref<TypeSpecifier> SemanticAnalyzer::UnifyArithmeticTypes(Ref<TypeSpecifier> a, Ref<TypeSpecifier> b)
    {
        if (*a == *b)
        {
            return a;
        }
        int rankA = RankTypes(a->GetType());
        int rankB = RankTypes(b->GetType());
        return (rankA >= rankB) ? a : b;
    }

    int RankTypes(Types t)
    {
        switch (t)
        {
        case Types::Double:
            return 5;
        case Types::Float:
            return 4;
        case Types::Int:
            return 3;
        case Types::Usize:
            return 2;
        case Types::Bool:
            return 1;
        default:
            return 0;
        }
    }

} // namespace Marble
