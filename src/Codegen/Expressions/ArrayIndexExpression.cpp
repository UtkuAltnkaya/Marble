#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"

namespace Marble
{
    llvm::Value *ArrayIndexExpression::Codegen(CodegenContext &codegenContext)
    {
        llvm::Value *elementAddress = Address(codegenContext);
        if (!elementAddress)
        {
            throw "Cannot find address of array";
        }

        llvm::Type *arrayType = m_ValueType->ToLLVMType(codegenContext);
        return codegenContext.Builder().CreateLoad(arrayType, elementAddress);
    }

    llvm::Value *ArrayIndexExpression::Address(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();

        llvm::Value *arrayValue = m_Array->Address(codegenContext);
        if (!arrayValue)
        {
            return nullptr;
        }

        llvm::Value *indexValue = m_Index->Codegen(codegenContext);
        if (!indexValue)
        {
            return nullptr;
        }

        llvm::Type *arrayType = m_Array->ValueType()->ToLLVMType(codegenContext);

        if (m_SecondIndex)
        {
            llvm::Value *secondIndexValue = m_SecondIndex->Codegen(codegenContext);
            if (!secondIndexValue)
            {
                return nullptr;
            }
            std::vector<llvm::Value *> indices = {builder.getInt64(0), indexValue, secondIndexValue};
            return builder.CreateInBoundsGEP(arrayType, arrayValue, indices);
        }
        std::vector<llvm::Value *> indices = {builder.getInt64(0), indexValue};
        return builder.CreateInBoundsGEP(arrayType, arrayValue, indices);
    }
} // namespace Marble