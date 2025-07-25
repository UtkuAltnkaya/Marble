#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"

namespace Marble
{
    llvm::Value *FunctionCallExpression::Codegen(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();
        IdentifierExpression *fnName = m_FnName->Into<IdentifierExpression>();

        llvm::Function *function = codegenContext.Module().getFunction(fnName->GetIdentifier().Id());

        ASSERT_D(function != nullptr, "Cannot find the function");

        std::vector<llvm::Value *> args;
        args.reserve(m_Args.size());

        for (size_t i = 0; i < m_Args.size(); ++i)
        {
            llvm::Value *argVal = m_Args[i]->Codegen(codegenContext);
            if (!argVal)
            {
                throw "Fail to evaluate argument expression";
            }
            args.push_back(argVal);
        }

        if (function->getReturnType()->isVoidTy())
        {
            return builder.CreateCall(function, args);
        }
        else
        {
            return builder.CreateCall(function, args);
        }
    }
} // namespace Marble
