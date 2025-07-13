#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    llvm::Value *ArrayInitExpression::Codegen(CodegenContext &codegenContext)
    {
        throw "Cannot create temporary array";
    }

    void ArrayInitExpression::CodegenInPlace(CodegenContext &codegenContext, llvm::AllocaInst *alloca)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::LLVMContext &context = codegenContext.Context();

        llvm::Type *arrayType = m_ValueType->ToLLVMType(codegenContext);

        for (size_t i = 0; i < m_Array.size(); ++i)
        {
            llvm::Value *value = m_Array[i]->Codegen(codegenContext);
            if (!value)
            {
                throw "Failed to evaluate array element";
            }

            llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            llvm::Value *index = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), static_cast<uint32_t>(i));
            llvm::Value *elementPtr = builder.CreateInBoundsGEP(arrayType, alloca, {zero, index});

            builder.CreateStore(value, elementPtr);
        }
    }
} // namespace Marble
