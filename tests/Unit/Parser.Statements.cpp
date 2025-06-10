#include <gtest/gtest.h>
#include "TestUtils/TestFixtures.hpp"
#include "TestUtils/MockClasses.hpp"
#include "TestUtils/Macros.hpp"

using namespace MarbleTest;

DEFINE_TEST_CASES(ParserFixture,
                  ParserLet, ParserLetWithType,
                  ParserReturn, ParserReturnVoid,
                  ParserDefer, ParserBlock,
                  ParserForWithLet, ParserForWithExpression,
                  ParserWhile, ParserSingleIf,
                  ParserIfWithElseIf, ParserIfWithElse)

TEST_P(ParserLet, Let)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &letStmt = block->Statements().at(0);
    auto let = dynamic_cast<const LetStatement *>(letStmt.get());
    ASSERT_NE(let, nullptr);

    EXPECT_EQ(let->GetIdentifier().Id(), "x");
    ASSERT_EQ(let->GetTypeSpecifier(), nullptr);
}

TEST_P(ParserLetWithType, LetWithTypeSpecifier)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &letStmt = block->Statements().at(0);
    auto let = dynamic_cast<const LetStatement *>(letStmt.get());
    ASSERT_NE(let, nullptr);

    auto typeSpecifier = MakeRef<TypeSpecifier>(ArrayType{MakeRef<TypeSpecifier>(Types::Int, Span{}), 3}, Span{});

    EXPECT_EQ(let->GetIdentifier().Id(), "x");
    EXPECT_EQ(*let->GetTypeSpecifier(), *typeSpecifier);
}

TEST_P(ParserReturn, Return)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &returnStmt = block->Statements().at(0);
    auto stmt = dynamic_cast<const ReturnStatement *>(returnStmt.get());
    ASSERT_NE(stmt, nullptr);
    ASSERT_NE(stmt->GetExpression(), nullptr);
}

TEST_P(ParserReturnVoid, ReturnVoid)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &returnStmt = block->Statements().at(0);
    auto stmt = dynamic_cast<const ReturnStatement *>(returnStmt.get());
    ASSERT_NE(stmt, nullptr);
    ASSERT_EQ(stmt->GetExpression(), nullptr);
}

TEST_P(ParserDefer, Defer)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &deferStmt = block->Statements().at(0);
    auto defer = dynamic_cast<const DeferStatement *>(deferStmt.get());
    ASSERT_NE(defer, nullptr);
    ASSERT_NE(&defer->GetExpression(), nullptr);
}
TEST_P(ParserBlock, Block)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &blockStmt = block->Statements().at(0);
    auto inBlock = dynamic_cast<const BlockStatement *>(blockStmt.get());

    ASSERT_NE(inBlock, nullptr);
    auto &stmts = inBlock->Statements();

    EXPECT_EQ(stmts.size(), 4);
    EXPECT_EQ(stmts[0]->StatementType(), StatementType::Let);
    EXPECT_EQ(stmts[1]->StatementType(), StatementType::Defer);
    EXPECT_EQ(stmts[2]->StatementType(), StatementType::Let);
    EXPECT_EQ(stmts[3]->StatementType(), StatementType::Return);
}

TEST_P(ParserForWithLet, ForWithLet)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &stmt = block->Statements().at(0);
    auto forStmt = dynamic_cast<const ForStatement *>(stmt.get());
    ASSERT_NE(forStmt, nullptr);

    EXPECT_NE(forStmt->GetLetStatement(), nullptr);
    EXPECT_EQ(forStmt->GetAssignmentExpression(), nullptr);

    EXPECT_EQ(forStmt->GetCondition().ExpressionType(), ExpressionType::Binary);
    EXPECT_EQ(forStmt->GetIncrement().ExpressionType(), ExpressionType::Unary);

    EXPECT_EQ(forStmt->GetBlock().StatementType(), StatementType::Block);
}

TEST_P(ParserForWithExpression, ForWithExpression)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &stmt = block->Statements().at(0);
    auto forStmt = dynamic_cast<const ForStatement *>(stmt.get());
    ASSERT_NE(forStmt, nullptr);

    EXPECT_EQ(forStmt->GetLetStatement(), nullptr);
    EXPECT_NE(forStmt->GetAssignmentExpression(), nullptr);
    EXPECT_EQ(forStmt->GetAssignmentExpression()->ExpressionType(), ExpressionType::Assignment);

    EXPECT_EQ(forStmt->GetCondition().ExpressionType(), ExpressionType::Binary);
    EXPECT_EQ(forStmt->GetIncrement().ExpressionType(), ExpressionType::Unary);

    EXPECT_EQ(forStmt->GetBlock().StatementType(), StatementType::Block);
}

TEST_P(ParserWhile, While)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &stmt = block->Statements().at(0);
    auto whileStmt = dynamic_cast<const WhileStatement *>(stmt.get());
    ASSERT_NE(whileStmt, nullptr);

    EXPECT_EQ(whileStmt->GetCondition().ExpressionType(), ExpressionType::Binary);
    EXPECT_EQ(whileStmt->GetBlock().StatementType(), StatementType::Block);
}

TEST_P(ParserSingleIf, SingleIf)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &stmt = block->Statements().at(0);
    auto ifStmt = dynamic_cast<const IfStatement *>(stmt.get());
    ASSERT_NE(ifStmt, nullptr);

    EXPECT_EQ(ifStmt->GetCondition().ExpressionType(), ExpressionType::Binary);
    EXPECT_EQ(ifStmt->GetBlock().StatementType(), StatementType::Block);
    EXPECT_EQ(ifStmt->GetElseIfStatements().size(), 0);
    ASSERT_EQ(ifStmt->GetElseStatement(), nullptr);
}

TEST_P(ParserIfWithElseIf, IfWithElseIf)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &stmt = block->Statements().at(0);
    auto ifStmt = dynamic_cast<const IfStatement *>(stmt.get());
    ASSERT_NE(ifStmt, nullptr);

    EXPECT_EQ(ifStmt->GetCondition().ExpressionType(), ExpressionType::Binary);
    EXPECT_EQ(ifStmt->GetBlock().StatementType(), StatementType::Block);
    EXPECT_EQ(ifStmt->GetElseIfStatements().size(), 2);
    ASSERT_EQ(ifStmt->GetElseStatement(), nullptr);

    auto &elseIfStmts = ifStmt->GetElseIfStatements();
    auto firstElseIf = dynamic_cast<const ElseIfStatement *>(elseIfStmts.at(0).get());
    ASSERT_NE(firstElseIf, nullptr);
    EXPECT_EQ(firstElseIf->GetCondition().ExpressionType(), ExpressionType::MemberAccess);
    EXPECT_EQ(ifStmt->GetBlock().StatementType(), StatementType::Block);

    auto secondElseIf = dynamic_cast<const ElseIfStatement *>(elseIfStmts.at(1).get());
    ASSERT_NE(secondElseIf, nullptr);
    EXPECT_EQ(secondElseIf->GetCondition().ExpressionType(), ExpressionType::Binary);
    EXPECT_EQ(ifStmt->GetBlock().StatementType(), StatementType::Block);
}

TEST_P(ParserIfWithElse, IfWithElse)
{
    auto block = PassUntilBlock();
    EXPECT_EQ(block->Statements().size(), 1);
    auto &stmt = block->Statements().at(0);
    auto ifStmt = dynamic_cast<const IfStatement *>(stmt.get());
    ASSERT_NE(ifStmt, nullptr);

    EXPECT_EQ(ifStmt->GetCondition().ExpressionType(), ExpressionType::Binary);
    EXPECT_EQ(ifStmt->GetBlock().StatementType(), StatementType::Block);
    EXPECT_EQ(ifStmt->GetElseIfStatements().size(), 0);
    ASSERT_NE(ifStmt->GetElseStatement(), nullptr);

    const auto elseStmt = dynamic_cast<const ElseStatement *const>(ifStmt->GetElseStatement());
    ASSERT_NE(elseStmt, nullptr);
    EXPECT_EQ(elseStmt->GetBlock().StatementType(), StatementType::Block);
}

INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserLet, ::testing::Values("fn main() -> void { let x = 12;}"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserLetWithType, ::testing::Values("fn main() -> void { let x: int[3] = [1,2,3];}"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserReturn, ::testing::Values("fn main() -> void { return x + y;}"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserReturnVoid, ::testing::Values("fn main() -> void { return; }"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserDefer, ::testing::Values("fn main() -> void { defer x.close(); }"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserBlock, ::testing::Values("fn main() -> void { { let x=1;defer x.close();let y=z.open();return;}}"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserForWithLet, ::testing::Values("fn main() -> void { for(let i = 1; i<10; i++) {x = x + 1;} }"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserForWithExpression, ::testing::Values("fn main() -> void { for(i = 1; i<10; i++) {x = x + 1;} }"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserWhile, ::testing::Values("fn main() -> void { while (x == 0) { y.parse();} }"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserSingleIf, ::testing::Values("fn main() -> void { if (x >= 0) { Namespace::Get(); } }"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserIfWithElseIf, ::testing::Values("fn main() -> void { if (x <= 0) { Namespace::Get(); } else if(x.is_true()) {object.get();} else if(x.y != 42) {return;} }"));
INSTANTIATE_TEST_SUITE_P(ParserStatements, ParserIfWithElse, ::testing::Values("fn main() -> void { if (x == 0  && y == 1) { Namespace::Get(); } else {object.get();}  }"));
