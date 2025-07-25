#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    llvm::Value *UnaryExpression::Codegen(CodegenContext &codegenContext)
    {
        switch (m_UnaryOperator)
        {
        case UnaryOperators::Plus:
        case UnaryOperators::Minus:
            return CodegenArithmetic(codegenContext);

        case UnaryOperators::LogicalNot:
        case UnaryOperators::BitwiseNot:
            return CodegenLogical(codegenContext);

        case UnaryOperators::Increment:
        case UnaryOperators::Decrement:
            return CodegenIncDec(codegenContext);

        case UnaryOperators::Address:
            return CodegenAddressOf(codegenContext);

        case UnaryOperators::Indirection:
            return CodegenDereference(codegenContext);

        default:
            throw "Unsupported unary operator in Codegen";
        }
    }

    llvm::Value *UnaryExpression::Address(CodegenContext &codegenContext)
    {
        if (m_UnaryOperator == UnaryOperators::Indirection)
        {
            return m_Value->Codegen(codegenContext);
        }
        return nullptr;
    }

    llvm::Value *UnaryExpression::CodegenArithmetic(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();

        llvm::Value *value = m_Value->Codegen(codegenContext);
        if (!value)
        {
            throw "Invalid operand for unary arithmetic expression";
        }

        llvm::Type *type = value->getType();
        switch (m_UnaryOperator)
        {
        case UnaryOperators::Plus:
            return value;

        case UnaryOperators::BitwiseNot:
        case UnaryOperators::Minus:
            if (type->isFloatingPointTy())
            {
                return builder.CreateFNeg(value, "neg_fp");
            }
            else if (type->isIntegerTy())
            {
                return builder.CreateNeg(value, "neg_int");
            }
            else
            {
                throw "Unsupported type for unary minus";
            }
        default:
            throw "Unknown unary arithmetic operator";
        }
    }

    llvm::Value *UnaryExpression::CodegenLogical(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::Value *value = m_Value->Codegen(codegenContext);
        if (!value)
        {
            throw "Failed to generate operand for unary logical";
        }

        llvm::Value *isZero = nullptr;
        llvm::Type *type = value->getType();
        if (type->isIntegerTy())
        {
            isZero = builder.CreateICmpEQ(value, llvm::ConstantInt::get(type, 0), "not_cmp");
        }
        else if (type->isFloatingPointTy())
        {
            isZero = builder.CreateFCmpOEQ(value, llvm::ConstantFP::get(type, 0.0), "not_cmp_fp");
        }
        else if (type->isPointerTy())
        {
            isZero = builder.CreateICmpEQ(value, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(type)), "not_cmp_ptr");
        }
        else
        {
            throw "Unsupported type for logical not";
        }

        return isZero;
    }

    llvm::Value *UnaryExpression::CodegenIncDec(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();

        llvm::Value *address = m_Value->Address(codegenContext);
        if (!address)
        {
            throw "Cannot get address for increment/decrement operation";
        }

        llvm::Type *elementType = m_Value->ValueType()->ToLLVMType(codegenContext);
        llvm::Value *typedAddress = builder.CreateBitCast(address, llvm::PointerType::get(elementType, 0));
        llvm::Value *loadedValue = builder.CreateLoad(elementType, typedAddress);

        llvm::Value *step = nullptr;
        if (elementType->isIntegerTy())
        {
            step = llvm::ConstantInt::get(elementType, 1);
        }
        else if (elementType->isFloatingPointTy())
        {
            step = llvm::ConstantFP::get(elementType, 1.0);
        }
        else
        {
            throw "Unsupported operator";
        }

        llvm::Value *newValue = nullptr;
        if (m_UnaryOperator == UnaryOperators::Increment)
        {
            if (elementType->isIntegerTy())
            {
                newValue = builder.CreateAdd(loadedValue, step, "inc");
            }
            else if (elementType->isFloatingPointTy())
            {
                newValue = builder.CreateFAdd(loadedValue, step, "inc");
            }
        }
        else
        {
            if (elementType->isIntegerTy())
            {
                newValue = builder.CreateSub(loadedValue, step, "dec");
            }
            else if (elementType->isFloatingPointTy())
            {
                newValue = builder.CreateFSub(loadedValue, step, "dec");
            }
        }

        builder.CreateStore(newValue, address);
        if (m_UnaryExpressionType == UnaryExpressionType::Prefix)
        {
            return newValue;
        }
        else
        {
            return loadedValue;
        }
    }

    llvm::Value *UnaryExpression::CodegenAddressOf(CodegenContext &codegenContext)
    {
        llvm::Value *ptr = m_Value->Address(codegenContext);

        if (!ptr || !ptr->getType()->isPointerTy())
        {
            throw "Cannot take address of non-lvalue";
        }

        return ptr;
    }

    llvm::Value *UnaryExpression::CodegenDereference(CodegenContext &codegenContext)
    {
        llvm::Value *pointer = m_Value->Codegen(codegenContext);
        Ref<TypeSpecifier> valueType = m_Value->ValueType();

        if (valueType->GetType() != Types::Pointer)
        {
            throw "Cannot dereference a non-pointer value";
        }

        llvm::Type *pointeeType = valueType->ToLLVMType(codegenContext);
        return codegenContext.Builder().CreateLoad(pointeeType, pointer);
    }

} // namespace Marble
