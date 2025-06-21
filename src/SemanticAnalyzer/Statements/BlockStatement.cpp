#include "Ast/Statements.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "Utils/IDGenerator.hpp"

namespace Marble
{
    Ref<TypeSpecifier> BlockStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        for (auto &statement : m_Statements)
        {
            if (statement->StatementType() == StatementType::Block)
            {
                SymbolNode *currentScope = table.CurrentScope();
                SymbolNode *newScope = new SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Block}, currentScope};
                currentScope->Insert("block-" + IDGenerator::Generate(), newScope);
                table.EnterScope(newScope);
                statement->Analyze(semanticAnalyzer);
                table.LeaveScope();
            }
            else
            {
                statement->Analyze(semanticAnalyzer);
            }
        }

        return TypeSpecifierOk;
    }

    void BlockStatement::SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        for (auto &statement : m_Statements)
        {
            statement->SubstituteGenerics(map);
        }
    }

    Box<Statement> BlockStatement::Clone()
    {
        return MakeBox<BlockStatement>(*this);
    }

} // namespace Marble
