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

        if (variableNode->IsTempVariable() && variableNode->GetAlloca() == nullptr)
        {
            llvm::IRBuilder<> &builder = codegenContext.Builder();
            llvm::Type *llvmType = variableNode->GetTypeSpecifier()->ToLLVMType(codegenContext);
            llvm::AllocaInst *alloca = builder.CreateAlloca(llvmType, nullptr);
            variableNode->SetAlloca(alloca);
            return alloca;
        }

        return variableNode->GetAlloca();
    }
} // namespace Marble
