#include <gtest/gtest.h>
#include "TestUtils/TestFixtures.hpp"
#include "TestUtils/MockClasses.hpp"
#include "TestUtils/Macros.hpp"

using namespace MarbleTest;

DEFINE_TEST_CASES(ParserFixture,
                  ParserBinaryExpressionSimple, ParserBinaryExpressionComplex,
                  ParserUnaryExpression)

TEST_P(ParserBinaryExpressionSimple, BinaryExpressionSimple)
{
    auto block = PassUntilBlock();
    auto &stmts = block->Statements();

    auto firstBinaryExpr = dynamic_cast<const BinaryExpression *>(CastToExpression(stmts.at(0).get()));
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&firstBinaryExpr->Left())->GetIdentifier().Id(), "x");
    EXPECT_EQ(firstBinaryExpr->Operator(), BinaryOperators::Add);
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&firstBinaryExpr->Right())->GetIdentifier().Id(), "y");

    auto secondBinaryExpr = dynamic_cast<const BinaryExpression *>(CastToExpression(stmts.at(1).get()));
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&secondBinaryExpr->Left())->GetIdentifier().Id(), "z");
    EXPECT_EQ(secondBinaryExpr->Operator(), BinaryOperators::Subtract);
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&secondBinaryExpr->Right())->GetIdentifier().Id(), "t");

    auto thirdBinaryExpr = dynamic_cast<const BinaryExpression *>(CastToExpression(stmts.at(2).get()));
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&thirdBinaryExpr->Left())->GetIdentifier().Id(), "a");
    EXPECT_EQ(thirdBinaryExpr->Operator(), BinaryOperators::Multiply);
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&thirdBinaryExpr->Right())->GetIdentifier().Id(), "b");

    auto forthBinaryExpr = dynamic_cast<const BinaryExpression *>(CastToExpression(stmts.at(3).get()));
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&forthBinaryExpr->Left())->GetIdentifier().Id(), "x");
    EXPECT_EQ(forthBinaryExpr->Operator(), BinaryOperators::Divide);
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&forthBinaryExpr->Right())->GetIdentifier().Id(), "y");

    auto fifthBinaryExpr = dynamic_cast<const BinaryExpression *>(CastToExpression(stmts.at(4).get()));
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&fifthBinaryExpr->Left())->GetIdentifier().Id(), "x");
    EXPECT_EQ(fifthBinaryExpr->Operator(), BinaryOperators::Modulo);
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&fifthBinaryExpr->Right())->GetIdentifier().Id(), "y");
}

TEST_P(ParserBinaryExpressionComplex, BinaryExpressionComplex)
{
    auto block = PassUntilBlock();
    auto &stmts = block->Statements();
    auto binaryExpr = dynamic_cast<const BinaryExpression *>(CastToExpression(stmts.at(0).get()));

    auto topLeft = dynamic_cast<const BinaryExpression *>(&binaryExpr->Left());

    auto topLeftFirst = dynamic_cast<const BinaryExpression *>(&topLeft->Left());
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&topLeftFirst->Left())->GetIdentifier().Id(), "x");
    EXPECT_EQ(topLeftFirst->Operator(), BinaryOperators::Multiply);
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&topLeftFirst->Right())->GetIdentifier().Id(), "y");
    EXPECT_EQ(topLeft->Operator(), BinaryOperators::Add);
    auto topLeftSecond = dynamic_cast<const BinaryExpression *>(&topLeft->Right());
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&topLeftSecond->Left())->GetIdentifier().Id(), "z");
    EXPECT_EQ(topLeftSecond->Operator(), BinaryOperators::Divide);
    EXPECT_EQ(dynamic_cast<const PrimitiveExpression *>(&topLeftSecond->Right())->GetValue(), "2");

    EXPECT_EQ(binaryExpr->Operator(), BinaryOperators::Add);

    auto topRight = dynamic_cast<const BinaryExpression *>(&binaryExpr->Right());
    ASSERT_NE(topRight, nullptr);
    ASSERT_NE(dynamic_cast<const IdentifierExpression *>(&topRight->Left()), nullptr);

    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&topRight->Left())->GetIdentifier().Id(), "x");
    EXPECT_EQ(topRight->Operator(), BinaryOperators::Subtract);

    auto topRightSecond = dynamic_cast<const BinaryExpression *>(&topRight->Right());
    ASSERT_NE(topRightSecond, nullptr);
    ASSERT_NE(dynamic_cast<const IdentifierExpression *>(&topRightSecond->Left()), nullptr);
    ASSERT_NE(dynamic_cast<const IdentifierExpression *>(&topRightSecond->Right()), nullptr);

    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&topRightSecond->Left())->GetIdentifier().Id(), "y");
    EXPECT_EQ(topRightSecond->Operator(), BinaryOperators::Multiply);
    EXPECT_EQ(dynamic_cast<const IdentifierExpression *>(&topRightSecond->Right())->GetIdentifier().Id(), "a");
}

TEST_P(ParserUnaryExpression, UnaryExpression)
{
    auto block = PassUntilBlock();
    auto &stmts = block->Statements();
    auto unaryExpr = dynamic_cast<const UnaryExpression *>(CastToExpression(stmts.at(0).get()));
}

INSTANTIATE_TEST_SUITE_P(ParserExpression, ParserBinaryExpressionSimple, ::testing::Values("fn main() -> void { x+y;z-t;a*b;x/y;x%y; }"));
INSTANTIATE_TEST_SUITE_P(ParserExpression, ParserBinaryExpressionComplex, ::testing::Values("fn main() -> void { (x * y + z / 2) + (x - y * a); }"));
INSTANTIATE_TEST_SUITE_P(ParserExpression, ParserUnaryExpression, ::testing::Values("fn main() -> void { *y; &x; x++; --y; ~z;  }"));