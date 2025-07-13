#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"

namespace Marble
{
    llvm::Value *ObjectInitExpression::Codegen(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::LLVMContext &context = codegenContext.Context();

        llvm::Type *type = m_ValueType->ToLLVMType(codegenContext);
        llvm::StructType *structType = llvm::dyn_cast<llvm::StructType>(type);
        if (!structType)
        {
            throw "Target is not a struct type";
        }

        llvm::AllocaInst *allocaInst = builder.CreateAlloca(structType, nullptr);
        CodegenInPlace(codegenContext, allocaInst);
        return builder.CreateLoad(structType, allocaInst);
    }

    void ObjectInitExpression::CodegenInPlace(CodegenContext &codegenContext, llvm::AllocaInst *alloca)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::LLVMContext &context = codegenContext.Context();

        llvm::Type *type = m_ValueType->ToLLVMType(codegenContext);
        llvm::StructType *structType = llvm::dyn_cast<llvm::StructType>(type);
        if (!structType)
        {
            throw "Target is not a struct type";
        }

        for (size_t i = 0; i < m_Fields.size(); ++i)
        {
            FieldExpression *fieldExpr = m_Fields.at(i)->Into<FieldExpression>();
            llvm::Value *value = fieldExpr->Codegen(codegenContext);
            llvm::Value *fieldPtr = builder.CreateStructGEP(structType, alloca, i);
            builder.CreateStore(value, fieldPtr);
        }
    }

    llvm::Value *FieldExpression::Codegen(CodegenContext &codegenContext)
    {
        return m_Value->Codegen(codegenContext);
    }

} // namespace Marble
