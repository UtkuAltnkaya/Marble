#include "Ast/Statements.hpp"
#include "Codegen/Codegen.hpp"

namespace Marble
{
    llvm::Value *ReturnStatement::Codegen(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::Function *function = builder.GetInsertBlock()->getParent();
        llvm::Type *returnType = function->getReturnType();

        if (m_Expression)
        {
            llvm::Value *value = m_Expression->Codegen(codegenContext);
            if (!value)
            {
                throw "Failed to evaluate return expression";
            }
            return builder.CreateRet(value);
        }
        return builder.CreateRetVoid();
    }
} // namespace Marble
