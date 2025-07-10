#include "Ast/Statements.hpp"
#include "Codegen/Codegen.hpp"

namespace Marble
{
    llvm::Value *BlockStatement::Codegen(CodegenContext &context)
    {
        llvm::IRBuilder<> &builder = context.Builder();

        llvm::Value *lastValue = nullptr;
        for (auto &statement : m_Statements)
        {
            if (statement->StatementType() == StatementType::Block)
            {
                // TODO
            }
            else
            {
                lastValue = statement->Codegen(context);
            }
        }
        return lastValue;
    }
} // namespace Marble
