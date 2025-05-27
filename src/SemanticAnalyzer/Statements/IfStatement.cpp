#include "Ast/Statements.hpp"
#include "Utils/IDGenerator.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{

    static void InsertAndAnalyzeBlock(const std::string &name, Statement &blockStatement);

    Ref<TypeSpecifier> IfStatement::Analyze()
    {
        Ref<TypeSpecifier> condition = m_Condition->Analyze();

        if (condition->GetType() != Types::Bool)
        {
            throw "Condition type must be boolean";
        }

        InsertAndAnalyzeBlock("if-", *m_Block.get());

        for (auto &elseIfStatement : m_ElseIfStatements)
        {
            elseIfStatement->Analyze();
        }

        if (m_ElseStatement)
        {
            m_ElseStatement->Analyze();
        }
        return TypeSpecifierOk;
    }

    Ref<TypeSpecifier> ElseIfStatement::Analyze()
    {
        Ref<TypeSpecifier> condition = m_Condition->Analyze();

        if (condition->GetType() != Types::Bool)
        {
            throw "Condition type must be boolean";
        }

        InsertAndAnalyzeBlock("else-if-", *m_Block.get());
        return TypeSpecifierOk;
    }

    Ref<TypeSpecifier> ElseStatement::Analyze()
    {
        InsertAndAnalyzeBlock("else-", *m_Block.get());
        return TypeSpecifierOk;
    }

    void InsertAndAnalyzeBlock(const std::string &name, Statement &blockStatement)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *parent = table.CurrentScope();
        SymbolNode *ifNode = new SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Block}, parent};
        parent->Insert(name + IDGenerator::Generate(), ifNode);

        table.EnterScope(ifNode);
        blockStatement.Analyze();
        table.LeaveScope();
    }
} // namespace Marble
