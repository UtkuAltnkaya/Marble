#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"

namespace Marble
{
    // TODO Add short circuiting
    // TODO Allow unsigned int operation
    llvm::Value *BinaryExpression::Codegen(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();

        llvm::Value *left = m_Left->Codegen(codegenContext);
        llvm::Value *right = m_Right->Codegen(codegenContext);

        if (!left || !right)
        {
            throw "Failed to evaluate operands";
        }

        bool isFloat = left->getType()->isFloatingPointTy();

                switch (m_Operator)
        {
        case BinaryOperators::Add:
            return isFloat ? builder.CreateFAdd(left, right, "faddtmp") : builder.CreateAdd(left, right, "addtmp");

        case BinaryOperators::Subtract:
            return isFloat ? builder.CreateFSub(left, right, "fsubtmp") : builder.CreateSub(left, right, "subtmp");

        case BinaryOperators::Multiply:
            return isFloat ? builder.CreateFMul(left, right, "fmultmp") : builder.CreateMul(left, right, "multmp");

        case BinaryOperators::Divide:
            return isFloat ? builder.CreateFDiv(left, right, "fdivtmp") : builder.CreateSDiv(left, right, "divtmp");

        case BinaryOperators::Modulo:
            return isFloat ? builder.CreateFRem(left, right, "fmodtmp") : builder.CreateSRem(left, right, "modtmp");

        case BinaryOperators::Equal:
            return isFloat ? builder.CreateFCmpUEQ(left, right, "cmptmp") : builder.CreateICmpEQ(left, right, "cmptmp");

        case BinaryOperators::NotEqual:
            return isFloat ? builder.CreateFCmpUNE(left, right, "cmptmp") : builder.CreateICmpNE(left, right, "cmptmp");

        case BinaryOperators::GreaterThan:
            return isFloat ? builder.CreateFCmpUGT(left, right, "cmptmp") : builder.CreateICmpSGT(left, right, "cmptmp");

        case BinaryOperators::LessThan:
            return isFloat ? builder.CreateFCmpULT(left, right, "cmptmp") : builder.CreateICmpSLT(left, right, "cmptmp");

        case BinaryOperators::GreaterThanOrEqual:
            return isFloat ? builder.CreateFCmpUGE(left, right, "cmptmp") : builder.CreateICmpSGE(left, right, "cmptmp");

        case BinaryOperators::LessThanOrEqual:
            return isFloat ? builder.CreateFCmpULE(left, right, "cmptmp") : builder.CreateICmpSLE(left, right, "cmptmp");

        case BinaryOperators::And:
            return builder.CreateAnd(left, right, "andtmp");

        case BinaryOperators::Or:
            return builder.CreateOr(left, right, "ortmp");

        case BinaryOperators::BitAnd:
            return builder.CreateAnd(left, right, "bitandtmp");

        case BinaryOperators::BitOr:
            return builder.CreateOr(left, right, "bitor");

        case BinaryOperators::BitXor:
            return builder.CreateXor(left, right, "bitxor");

        case BinaryOperators::BitLeft:
            return builder.CreateShl(left, right, "shltmp");

        case BinaryOperators::BitRight:
            return builder.CreateAShr(left, right, "ashrtmp");
        default:
            throw "Unknown binary operator";
        }
    }
} // namespace Marble
