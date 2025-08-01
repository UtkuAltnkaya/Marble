#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"
#include "Utils/Macros.hpp"
#include "Utils/IDGenerator.hpp"
#include "Utils/File.hpp"

namespace Marble
{
    static int RankTypes(Types t);

    SemanticAnalyzer::SemanticAnalyzer(Ref<Program> program, const Marble::File &file) : m_Program{program}, m_File{file}
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

    const std::string &SemanticAnalyzer::InstantiateGenerics(const std::string &name, const Generics *generics)
    {
        // ASSERT_A(generics != nullptr, "Generic must be provided");
        // return InstantiateGenerics(name, generics->Types());
    }

    const std::string &SemanticAnalyzer::InstantiateGenerics(const std::string &name, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        // ExpandNestedGenerics(typeArgs);

        // SymbolTable &table = SymbolTable::Get();
        // SymbolNode *scope = table.CurrentScope();
        // SymbolNode *parent = scope->Iter().Parent().Find();

        // GenericInstanceKey key;

        // if (auto node = parent->Iter().Function(name).Find(); node)
        // {
        //     Box<Definition> expandedFnDefinition = Instantiate(node, key, typeArgs);
        //     if (!expandedFnDefinition)
        //     {
        //         return m_Generis[key];
        //     }

        //     FunctionSymbolNode *newFnSymbol = new FunctionSymbolNode{*expandedFnDefinition->Into<FunctionDefinition>(),
        //                                                              parent};
        //     parent->Insert(expandedFnDefinition->GetName(), newFnSymbol);
        //     AddExpandedDefinition(std::move(expandedFnDefinition));
        //     return m_Generis[key];
        // }

        // if (auto node = table.Root()->Iter().Struct(name).Find(); node)
        // {
        //     Box<Definition> expandedStructDefinition = Instantiate(node, key, typeArgs);
        //     if (!expandedStructDefinition)
        //     {
        //         return m_Generis[key];
        //     }
        //     StructDefinition *newStructDefinition = expandedStructDefinition->Into<StructDefinition>();
        //     SymbolNode *newStructNode = new StructSymbolNode{*newStructDefinition, table.Root()};
        //     table.Insert(expandedStructDefinition->GetName(), newStructNode);

        //     ImplDefinition *implDefinition = newStructDefinition->GetImplDefinition();
        //     if (!implDefinition)
        //     {
        //         return m_Generis[key];
        //     }
        //     Box<Definition> expandedImplDefinition = implDefinition->InstantiateWith(*this, typeArgs);
        //     ImplDefinition *newImplDefinition = expandedImplDefinition->Into<ImplDefinition>();

        //     Ref<TypeSpecifier> implName = MakeRef<TypeSpecifier>(
        //         Identifier{newStructDefinition->GetName(), newStructDefinition->GetIdentifier().GetSpan()},
        //         newStructDefinition->GetIdentifier().GetSpan());

        //     newImplDefinition->SetImplName(implName);
        //     newImplDefinition->CreateSymbol();
        //     AddExpandedDefinition(std::move(expandedStructDefinition));
        //     AddExpandedDefinition(std::move(expandedImplDefinition));
        //     return m_Generis[key];
        // }

        // if (auto node = scope->Iter().Function(name).Find(); node)
        // {
        //     Box<Definition> expandedMethodDefinition = Instantiate(node, key, typeArgs);
        //     if (!expandedMethodDefinition)
        //     {
        //         return m_Generis[key];
        //     }
        //     MemberFunctionDefinition *newMemberFunctionDefinition = expandedMethodDefinition->Into<MemberFunctionDefinition>();
        //     FunctionSymbolNode *newFunctionNode = new FunctionSymbolNode{*newMemberFunctionDefinition, scope};
        //     scope->Insert(newMemberFunctionDefinition->GetName(), newFunctionNode);

        //     expandedMethodDefinition->Analyze(*this);

        //     Definition *definition = static_cast<Definition *>(scope->GetAstPtr());
        //     StructDefinition *structDefinition = definition->Into<StructDefinition>();
        //     ImplDefinition *implDefinition = structDefinition->GetImplDefinition();
        //     implDefinition->AddMemberFunction(Box<MemberFunctionDefinition>(expandedMethodDefinition.release()->Into<MemberFunctionDefinition>()));
        //     return m_Generis[key];
        // }
        // ASSERT_D(false, "Something went wrong");
        // UNREACHABLE();
    }

    Box<Definition> SemanticAnalyzer::Instantiate(SymbolNode *node, GenericInstanceKey &key, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {

        // Definition *definition = static_cast<Definition *>(node->GetAstPtr());
        // GenerateGenericKey(key, definition, typeArgs);
        // if (m_Generis.contains(key))
        // {
        //     return nullptr;
        // }
        // Box<Identifier> id = CreateExpandedName(definition->GetIdentifier(), typeArgs);
        // m_Generis[key] = id->Id();
        // Box<Definition> expandedDefinition = definition->InstantiateWith(*this, typeArgs);
        // expandedDefinition->SetName(std::move(id));
        // return expandedDefinition;
    }

    Box<Identifier> SemanticAnalyzer::CreateExpandedName(const Identifier &id, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        // std::string finalName = id.Id();
        // for (auto &type : typeArgs)
        // {
        //     finalName += "_" + type->ToString();
        // }
        // finalName += "_" + IDGenerator::Generate();

        // return MakeBox<Identifier>(finalName, id.GetSpan());
    }

      void SemanticAnalyzer::GenerateGenericKey(GenericInstanceKey &key, const Definition *definition, const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        // key.Name = definition->GetName();
        // key.KeyType = GenericInstanceKey::FromDefinitionType((int)definition->DefinitionType());
        // for (auto &type : typeArgs)
        // {
        //     key.TypeArgumentNames.emplace_back(type->ToString());
        // }
    }

    void SemanticAnalyzer::ExpandNestedGenerics(const std::vector<Ref<TypeSpecifier>> &typeArgs)
    {
        // auto &vector = const_cast<std::vector<Ref<TypeSpecifier>> &>(typeArgs);
        // size_t size = typeArgs.size();
        // for (int i = 0; i < size; i++)
        // {
        //     auto &type = vector[i];
        //     if (type->GetType() != Types::GenericType)
        //     {
        //         continue;
        //     }
        //     auto &nestedGeneric = type->Generic();
        //     const std::string &name = InstantiateGenerics(nestedGeneric.OuterType.Id(), nestedGeneric.InnerType);
        //     Span span;
        //     span.Start = nestedGeneric.OuterType.GetSpan().Start;
        //     span.End = Position{span.Start.Row, span.Start.Col + name.size(), span.Start.Cursor + name.size()};
        //     auto expanded = MakeRef<TypeSpecifier>(Identifier{name, span}, span);
        //     vector[i].swap(expanded);
        // }
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
