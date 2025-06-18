#include "Ast/Statements.hpp"
#include "Utils/IDGenerator.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> WhileStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        Ref<TypeSpecifier> condition = m_Condition->Analyze(semanticAnalyzer);

        // TODO: Decide whether conditions must be bool or bool and int
        if (condition->GetType() != Types::Bool /*&& condition->GetType() != Types::Int*/)
        {
            throw "Condition type must be boolean";
        }

        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *parent = table.CurrentScope();
        SymbolNode *whileNode = new SymbolNode{SymbolData{SymbolAccess::Local, SymbolNodeTypes::Block}, parent};
        parent->Insert("while-" + IDGenerator::Generate(), whileNode);

        table.EnterScope(whileNode);
        m_Block->Analyze(semanticAnalyzer);
        table.LeaveScope();
        return TypeSpecifierOk;
    }

    Box<Statement> WhileStatement::Clone()
    {
        return MakeBox<WhileStatement>(*this);
    }
} // namespace Marble