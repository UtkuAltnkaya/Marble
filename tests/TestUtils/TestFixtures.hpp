#pragma once

#include <gtest/gtest.h>

#include "Lexer/Lexer.hpp"
#include "Parser/Parser.hpp"
#include "Ast/Program.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "TestUtils/MockClasses.hpp"

namespace MarbleTest
{
    using namespace Marble;

    class LexerFixture : public ::testing::TestWithParam<std::string>
    {
    protected:
        void SetUp() override
        {
            m_File = MakeBox<MockFile>(GetParam());
            m_Lexer = MakeBox<Lexer>(*m_File.get());
        }

        std::vector<Box<Token>> GetAllTokens()
        {
            std::vector<Box<Token>> tokens;
            while (true)
            {
                auto token = m_Lexer->NextToken();
                auto tokenType = token->TokenType();
                tokens.push_back(std::move(token));
                if (tokenType == TokenType::Eof)
                {
                    break;
                }
            }
            return tokens;
        }

        void MatchTokens(const std::vector<Box<Token>> &inputTokens, const std::vector<Box<Token>> &expectedTokens)
        {
            ASSERT_EQ(inputTokens.size(), expectedTokens.size());
            for (size_t i = 0; i < inputTokens.size(); i++)
            {
                EXPECT_EQ(inputTokens[i]->TokenType(), expectedTokens[i]->TokenType());
            }
        }

        Box<Lexer> m_Lexer;
        Box<File> m_File;
    };

    class ParserFixture : public ::testing::TestWithParam<std::string>
    {
    protected:
        void SetUp() override
        {
            SymbolTable::Init();
            m_File = MakeBox<MockFile>(GetParam());
            m_Lexer = MakeBox<Lexer>(*m_File.get());
            m_Parser = MakeBox<Parser>(*m_Lexer.get());
            try
            {
                m_Program = m_Parser->Parse();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Unexpected std::exception: " << e.what();
            }
            catch (const std::string &e)
            {
                std::cerr << "Unexpected std::exception: " << e;
            }
            catch (const char *e)
            {
                std::cerr << "Unexpected std::exception: " << e;
            }
            ASSERT_NE(m_Program, nullptr) << "Parse returned nullptr";
        }

        void TearDown() override
        {
            SymbolTable::ShutDown();
        }

        const BlockStatement *PassUntilBlock()
        {
            auto &defs = m_Program->Definitions();
            EXPECT_EQ(defs.size(), 1);
            auto &fnDefinition = defs.at(0);
            auto castedFnDef = dynamic_cast<FunctionDefinition *>(fnDefinition.get());
            EXPECT_NE(castedFnDef, nullptr);

            const BlockStatement *block = dynamic_cast<const BlockStatement *>(&castedFnDef->GetBody());
            EXPECT_NE(block, nullptr);
            return block;
        }

        const Expression *CastToExpression(const Statement *statement)
        {
            auto exprStmt = dynamic_cast<const ExpressionStatement *>(statement);
            EXPECT_NE(exprStmt, nullptr);
            return &exprStmt->GetExpression();
        }

        Box<File> m_File;
        Box<Lexer> m_Lexer;
        Box<Parser> m_Parser;
        Ref<Program> m_Program;
    };

} // namespace MarbleTes
