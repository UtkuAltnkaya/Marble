#include "Ast/Statements.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "Utils/IDGenerator.hpp"

namespace Marble
{
    Ref<TypeSpecifier> BlockStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::Get();
        for (auto &statement : m_Statements)
        {
            if (statement->StatementType() == StatementType::Block)
            {
                SymbolNode *currentScope = table.CurrentScope();
                BlockSymbolNode *newScope = new BlockSymbolNode("block_" + IDGenerator::Generate(), currentScope);
                currentScope->Insert(newScope);
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

    void BlockStatement::SubstituteGenerics(
        GenericExpander &genericExpander, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        for (auto &statement : m_Statements)
        {
            statement->SubstituteGenerics(genericExpander, map);
        }
    }

    Box<Statement> BlockStatement::Clone()
    {
        return MakeBox<BlockStatement>(*this);
    }

} // namespace Marble
