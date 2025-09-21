#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    llvm::Value *IdentifierExpression::Codegen(CodegenContext &codegenContext)
    {
        auto &builder = codegenContext.Builder();

        llvm::Value *allocaVal = Address(codegenContext);
        if (!allocaVal)
        {
            return nullptr;
        }
        llvm::AllocaInst *alloca = llvm::cast<llvm::AllocaInst>(allocaVal);
        return builder.CreateLoad(alloca->getAllocatedType(), alloca);
    }

    llvm::Value *IdentifierExpression::Address(CodegenContext &codegenContext)
    {
        SymbolNode *node = FindNode();
        VariableSymbolNode *variableNode = node->Into<VariableSymbolNode>();

        llvm::AllocaInst *alloca = variableNode->Alloca();

        if (alloca)
        {
            return alloca;
        }

        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::Type *llvmType = variableNode->GetTypeSpecifier()->ToLLVMType(codegenContext);
        alloca = builder.CreateAlloca(llvmType, nullptr);
        variableNode->Alloca(alloca);
        return alloca;
    }
} // namespace Marble
