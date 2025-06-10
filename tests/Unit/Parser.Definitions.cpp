#include <gtest/gtest.h>
#include "TestUtils/TestFixtures.hpp"
#include "TestUtils/MockClasses.hpp"
#include "TestUtils/Macros.hpp"

using namespace MarbleTest;

DEFINE_TEST_CASES(ParserFixture,
                  ParserFunction,
                  ParserFunctionWithParams,
                  ParserStruct,
                  ParserStructWithGeneric,
                  ParserEnum,
                  ParserImpl,
                  ParserImplWithGenerics)

TEST_P(ParserFunction, Function)
{
    auto &defs = m_Program->Definitions();

    EXPECT_EQ(defs.size(), 1);

    auto &fnDefinition = defs.at(0);
    auto castedFnDef = dynamic_cast<FunctionDefinition *>(fnDefinition.get());
    ASSERT_NE(castedFnDef, nullptr);

    auto expected = MakeBox<FunctionDefinition>(
        AccessSpecifier::Private,
        MakeBox<Identifier>("main", Span{}),
        nullptr,
        std::vector<Box<VariableType>>(),
        MakeRef<TypeSpecifier>(Types::Void),
        nullptr,
        Span{});

    EXPECT_EQ(castedFnDef->GetAccessSpecifier(), expected->GetAccessSpecifier());
    EXPECT_EQ(castedFnDef->GetName(), expected->GetName());
    ASSERT_EQ(castedFnDef->GetGenerics(), nullptr);
    EXPECT_EQ(castedFnDef->GetParams().size(), expected->GetParams().size());
    for (size_t i = 0; i < castedFnDef->GetParams().size(); ++i)
    {
        EXPECT_EQ(*castedFnDef->GetParams()[i], *expected->GetParams()[i]);
    }
    EXPECT_EQ(*castedFnDef->GetReturnType(), *expected->GetReturnType());
}

TEST_P(ParserFunctionWithParams, FunctionWithParams)
{
    auto &defs = m_Program->Definitions();
    EXPECT_EQ(defs.size(), 1);

    auto &fnDefinition = defs.at(0);
    auto castedFnDef = dynamic_cast<FunctionDefinition *>(fnDefinition.get());
    ASSERT_NE(castedFnDef, nullptr);

    std::vector<Box<VariableType>> params;
    params.emplace_back(MakeBox<VariableType>(MakeBox<Identifier>("x", Span{}), MakeRef<TypeSpecifier>(Types::Int), Span{}));
    params.emplace_back(MakeBox<VariableType>(MakeBox<Identifier>("y", Span{}), MakeRef<TypeSpecifier>(Types::Float), Span{}));
    auto expected = MakeBox<FunctionDefinition>(
        AccessSpecifier::Private,
        MakeBox<Identifier>("add", Span{}),
        nullptr,
        std::move(params),
        MakeRef<TypeSpecifier>(Types::Double),
        nullptr,
        Span{});

    EXPECT_EQ(castedFnDef->GetAccessSpecifier(), expected->GetAccessSpecifier());
    EXPECT_EQ(castedFnDef->GetName(), expected->GetName());
    ASSERT_EQ(castedFnDef->GetGenerics(), nullptr);
    EXPECT_EQ(castedFnDef->GetParams().size(), expected->GetParams().size());
    for (size_t i = 0; i < castedFnDef->GetParams().size(); ++i)
    {
        EXPECT_EQ(*castedFnDef->GetParams()[i], *expected->GetParams()[i]);
    }
    EXPECT_EQ(*castedFnDef->GetReturnType(), *expected->GetReturnType());
}

TEST_P(ParserStruct, Struct)
{
    auto &defs = m_Program->Definitions();
    EXPECT_EQ(defs.size(), 1);
    auto &structDefinition = defs.at(0);
    auto castedStructDef = dynamic_cast<StructDefinition *>(structDefinition.get());
    ASSERT_NE(castedStructDef, nullptr);

    std::vector<Box<StructFieldDefinition>> fields;
    fields.emplace_back(MakeBox<StructFieldDefinition>(
        AccessSpecifier::Public,
        MakeBox<VariableType>(MakeBox<Identifier>("name", Span{}), MakeRef<TypeSpecifier>(Types::Str, Span{}), Span{}),
        Span{}));
    fields.emplace_back(MakeBox<StructFieldDefinition>(
        AccessSpecifier::Private,
        MakeBox<VariableType>(MakeBox<Identifier>("age", Span{}), MakeRef<TypeSpecifier>(Types::Int, Span{}), Span{}),
        Span{}));
    auto expected = MakeBox<StructDefinition>(
        AccessSpecifier::Public,
        MakeBox<Identifier>("Person", Span{}),
        nullptr,
        std::move(fields),
        Span{});

    EXPECT_EQ(castedStructDef->GetAccessSpecifier(), expected->GetAccessSpecifier());
    EXPECT_EQ(castedStructDef->GetName(), expected->GetName());
    ASSERT_EQ(castedStructDef->GetGenerics(), nullptr);
    EXPECT_EQ(castedStructDef->GetFields().size(), expected->GetFields().size());
    for (size_t i = 0; i < castedStructDef->GetFields().size(); ++i)
    {
        auto &l = castedStructDef->GetFields()[i];
        auto &r = expected->GetFields()[i];
        EXPECT_EQ(l->GetAccessSpecifier(), r->GetAccessSpecifier());
        EXPECT_EQ(l->GetField(), r->GetField());
    }
}

TEST_P(ParserStructWithGeneric, StructWithGenerics)
{
    auto &defs = m_Program->Definitions();
    EXPECT_EQ(defs.size(), 1);
    auto &structDefinition = defs.at(0);
    auto castedStructDef = dynamic_cast<StructDefinition *>(structDefinition.get());
    ASSERT_NE(castedStructDef, nullptr);

    std::vector<Box<StructFieldDefinition>> fields;
    fields.emplace_back(MakeBox<StructFieldDefinition>(
        AccessSpecifier::Private,
        MakeBox<VariableType>(MakeBox<Identifier>("first", Span{}),
                              MakeRef<TypeSpecifier>(PointerType{MakeRef<TypeSpecifier>(Identifier("T", Span{}), Span{})}, Span{}), Span{}),
        Span{}));
    fields.emplace_back(MakeBox<StructFieldDefinition>(
        AccessSpecifier::Private,
        MakeBox<VariableType>(MakeBox<Identifier>("second", Span{}),
                              MakeRef<TypeSpecifier>(Identifier{"U", Span{}}, Span{}), Span{}),
        Span{}));

    std::vector<Ref<TypeSpecifier>> generics;
    generics.emplace_back(MakeRef<TypeSpecifier>(Identifier("T", Span{}), Span{}));
    generics.emplace_back(MakeRef<TypeSpecifier>(Identifier("U", Span{}), Span{}));

    auto expected = MakeBox<StructDefinition>(
        AccessSpecifier::Public,
        MakeBox<Identifier>("Pair", Span{}),
        MakeBox<Generics>(std::move(generics), Span{}),
        std::move(fields),
        Span{});

    EXPECT_EQ(castedStructDef->GetAccessSpecifier(), expected->GetAccessSpecifier());
    EXPECT_EQ(castedStructDef->GetName(), expected->GetName());
    EXPECT_EQ(*castedStructDef->GetGenerics(), *expected->GetGenerics());
    EXPECT_EQ(castedStructDef->GetFields().size(), expected->GetFields().size());
    for (size_t i = 0; i < castedStructDef->GetFields().size(); ++i)
    {
        auto &l = castedStructDef->GetFields()[i];
        auto &r = expected->GetFields()[i];
        EXPECT_EQ(l->GetAccessSpecifier(), r->GetAccessSpecifier());
        EXPECT_EQ(l->GetField(), r->GetField());
    }
}

TEST_P(ParserEnum, Enum)
{
    auto &defs = m_Program->Definitions();
    EXPECT_EQ(defs.size(), 1);
    auto &enumDefinition = defs.at(0);

    auto castedEnumDef = dynamic_cast<EnumDefinition *>(enumDefinition.get());
    ASSERT_NE(castedEnumDef, nullptr);

    EXPECT_EQ(castedEnumDef->GetName(), "Types");

    auto &fields = castedEnumDef->GetFields();
    EXPECT_EQ(fields.size(), 2);

    EXPECT_EQ(fields[0]->Id(), "Int");
    EXPECT_EQ(fields[1]->Id(), "Float");
}

TEST_P(ParserImpl, Impl)
{
    auto &defs = m_Program->Definitions();
    EXPECT_EQ(defs.size(), 1);
    auto &implDefinition = defs.at(0);
    auto castedImplDef = dynamic_cast<ImplDefinition *>(implDefinition.get());
    ASSERT_NE(castedImplDef, nullptr);

    EXPECT_EQ(castedImplDef->GetName()->GetType(), Types::UserDefine);
    EXPECT_EQ(castedImplDef->GetName()->UserDefine().Id(), "Person");
    EXPECT_EQ(castedImplDef->GetMemberFunctions().size(), 1);
    ASSERT_EQ(castedImplDef->GetGenerics(), nullptr);

    auto &memberFunction = castedImplDef->GetMemberFunctions().at(0);
    auto &prototype = memberFunction->GetPrototype();
    ASSERT_EQ(prototype.GetMethod(), nullptr);
    EXPECT_EQ(prototype.GetName().Id(), "new");
    EXPECT_EQ(prototype.GetParams().size(), 0);
    EXPECT_EQ(prototype.GetReturnType()->GetType(), Types::UserDefine);
    EXPECT_EQ(prototype.GetReturnType()->UserDefine().Id(), "Person");
}

TEST_P(ParserImplWithGenerics, ImplWithGenerics)
{
    auto &defs = m_Program->Definitions();
    EXPECT_EQ(defs.size(), 1);
    auto &implDefinition = defs.at(0);
    auto castedImplDef = dynamic_cast<ImplDefinition *>(implDefinition.get());
    ASSERT_NE(castedImplDef, nullptr);

    EXPECT_EQ(castedImplDef->GetName()->GetType(), Types::UserDefine);
    EXPECT_EQ(castedImplDef->GetName()->UserDefine().Id(), "Stack");

    ASSERT_NE(castedImplDef->GetGenerics(), nullptr);
    EXPECT_EQ(castedImplDef->GetGenerics()->Types().size(), 1);
    EXPECT_EQ(castedImplDef->GetGenerics()->Types().at(0)->GetType(), Types::UserDefine);
    EXPECT_EQ(castedImplDef->GetGenerics()->Types().at(0)->UserDefine().Id(), "T");

    EXPECT_EQ(castedImplDef->GetMemberFunctions().size(), 1);

    auto &memberFunction = castedImplDef->GetMemberFunctions().at(0);
    auto &prototype = memberFunction->GetPrototype();

    ASSERT_NE(prototype.GetMethod(), nullptr);
    EXPECT_EQ(prototype.GetMethod()->GetIdentifier().Id(), "s");
    EXPECT_EQ(prototype.GetMethod()->GetTypeSpecifier()->GetType(), Types::Pointer);

    auto pointerType = prototype.GetMethod()->GetTypeSpecifier()->Pointer().TypeSpecifier;

    EXPECT_EQ(pointerType->GetType(), Types::GenericType);
    EXPECT_EQ(pointerType->Generic().OuterType.Id(), "Stack");
    EXPECT_EQ(pointerType->Generic().InnerType.size(), 1);
    EXPECT_EQ(pointerType->Generic().InnerType.at(0)->GetType(), Types::UserDefine);
    EXPECT_EQ(pointerType->Generic().InnerType.at(0)->UserDefine().Id(), "T");
}

INSTANTIATE_TEST_SUITE_P(ParserDefinitions, ParserFunction, ::testing::Values("fn main() -> void {}"));
INSTANTIATE_TEST_SUITE_P(ParserDefinitions, ParserFunctionWithParams, ::testing::Values("fn add(x:int, y:float) -> double {}"));
INSTANTIATE_TEST_SUITE_P(ParserDefinition, ParserStruct, ::testing::Values("pub struct Person {pub name:str,age:int}"));
INSTANTIATE_TEST_SUITE_P(ParserDefinition, ParserStructWithGeneric, ::testing::Values("pub struct Pair<T,U> {first: T*,second:U}"));
INSTANTIATE_TEST_SUITE_P(ParserDefinition, ParserEnum, ::testing::Values("pub enum Types {Int,Float}"));
INSTANTIATE_TEST_SUITE_P(ParserDefinition, ParserImpl, ::testing::Values("impl Person { fn new() -> Person {}}"));
INSTANTIATE_TEST_SUITE_P(ParserDefinition, ParserImplWithGenerics, ::testing::Values("impl Stack<T> { fn (s:Stack<T>*) get_data<T>() -> T* {}}"));
