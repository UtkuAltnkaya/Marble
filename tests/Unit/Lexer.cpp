#include <gtest/gtest.h>
#include <string>
#include "TestUtils/TestFixtures.hpp"
#include "TestUtils/MockClasses.hpp"
#include "TestUtils/Macros.hpp"

using namespace MarbleTest;

DEFINE_TEST_CASES(LexerFixture, LexerSingleIdentifier, LexerMultipleTokens, LexerOperators, LexerDeliminator)

TEST_P(LexerSingleIdentifier, SingleIdentifier)
{
    auto tokens = GetAllTokens();

    EXPECT_EQ(tokens.size(), 2);

    std::vector<Box<Token>> expectedTokens;
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::For));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Eof));

    MatchTokens(tokens, expectedTokens);
}

TEST_P(LexerMultipleTokens, MultipleTokens)
{
    auto tokens = GetAllTokens();

    EXPECT_EQ(tokens.size(), 9);

    std::vector<Box<Token>> expectedTokens;
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Fn));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Identifier));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::OpenParen));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::CloseParen));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Arrow));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Void));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::OpenCurlyBrace));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::CloseCurlyBrace));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Eof));

    MatchTokens(tokens, expectedTokens);
}

TEST_P(LexerOperators, Operators)
{
    auto tokens = GetAllTokens();

    EXPECT_EQ(tokens.size(), 19);

    std::vector<Box<Token>> expectedTokens;
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Plus));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Minus));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Multiply));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Divide));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::GreaterThan));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::LessThan));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Percent));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::BitAnd));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::BitOr));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Assign));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::And));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Or));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::GreaterEqual));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::LessEqual));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Tilde));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::BitRight));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::BitXor));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::BitLeft));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Eof));

    MatchTokens(tokens, expectedTokens);
}

TEST_P(LexerDeliminator, Deliminator)
{
    auto tokens = GetAllTokens();

    EXPECT_EQ(tokens.size(), 11);

    std::vector<Box<Token>> expectedTokens;
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::OpenParen));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::CloseParen));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::OpenBracket));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::CloseBracket));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::OpenCurlyBrace));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::CloseCurlyBrace));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Comma));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Colon));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Semicolon));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Dot));
    expectedTokens.emplace_back(MakeBox<MockToken>(TokenType::Eof));

    MatchTokens(tokens, expectedTokens);
}

INSTANTIATE_TEST_SUITE_P(LexerFixtures, LexerSingleIdentifier, ::testing::Values("for"));
INSTANTIATE_TEST_SUITE_P(LexerFixtures, LexerMultipleTokens, ::testing::Values("fn main () -> void {}"));
INSTANTIATE_TEST_SUITE_P(LexerFixtures, LexerOperators, ::testing::Values("+-*/><%&|=&&||>=<=~>>^<<"));
INSTANTIATE_TEST_SUITE_P(LexerFixtures, LexerDeliminator, ::testing::Values("()[]{},:;."));