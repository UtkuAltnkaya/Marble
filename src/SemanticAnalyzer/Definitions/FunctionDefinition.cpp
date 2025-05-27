#include "Ast/Definitions.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    Ref<TypeSpecifier> FunctionDefinition::Analyze()
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *iter = table.Iter()
                               .Function(m_FunctionName->Id())
                               .Ok<FunctionDefinition>(this)
                               .Find();

        table.EnterScope(iter);
        m_Block->Analyze();
        table.LeaveScope();

        if (m_ReturnType->GetType() == Types::Void)
        {
            return TypeSpecifierOk;
        }

        BlockStatement *block = static_cast<BlockStatement *>(m_Block.get());

        const std::vector<Box<Statement>> &statements = block->Statements();
        if (statements.size() == 0)
        {
            // TODO: Decide to allow empty function body or not, or warn
        }

        if (statements.back()->StatementType() == StatementType::Return)
        {
            return TypeSpecifierOk;
        }

        throw "Return statement expected";
    }
} // namespace Marble
