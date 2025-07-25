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
            return isFloat ? builder.CreateFAdd(left, right) : builder.CreateAdd(left, right);

        case BinaryOperators::Subtract:
            return isFloat ? builder.CreateFSub(left, right) : builder.CreateSub(left, right);

        case BinaryOperators::Multiply:
            return isFloat ? builder.CreateFMul(left, right) : builder.CreateMul(left, right);

        case BinaryOperators::Divide:
            return isFloat ? builder.CreateFDiv(left, right) : builder.CreateSDiv(left, right);

        case BinaryOperators::Modulo:
            return isFloat ? builder.CreateFRem(left, right) : builder.CreateSRem(left, right);

        case BinaryOperators::Equal:
            return isFloat ? builder.CreateFCmpUEQ(left, right) : builder.CreateICmpEQ(left, right);

        case BinaryOperators::NotEqual:
            return isFloat ? builder.CreateFCmpUNE(left, right) : builder.CreateICmpNE(left, right);

        case BinaryOperators::GreaterThan:
            return isFloat ? builder.CreateFCmpUGT(left, right) : builder.CreateICmpSGT(left, right);

        case BinaryOperators::LessThan:
            return isFloat ? builder.CreateFCmpULT(left, right) : builder.CreateICmpSLT(left, right);

        case BinaryOperators::GreaterThanOrEqual:
            return isFloat ? builder.CreateFCmpUGE(left, right) : builder.CreateICmpSGE(left, right);

        case BinaryOperators::LessThanOrEqual:
            return isFloat ? builder.CreateFCmpULE(left, right) : builder.CreateICmpSLE(left, right);

        case BinaryOperators::And:
            return builder.CreateAnd(left, right);

        case BinaryOperators::Or:
            return builder.CreateOr(left, right);

        case BinaryOperators::BitAnd:
            return builder.CreateAnd(left, right);

        case BinaryOperators::BitOr:
            return builder.CreateOr(left, right);

        case BinaryOperators::BitXor:
            return builder.CreateXor(left, right);

        case BinaryOperators::BitLeft:
            return builder.CreateShl(left, right);

        case BinaryOperators::BitRight:
            return builder.CreateAShr(left, right);
        default:
            throw "Unknown binary operator";
        }
    }
} // namespace Marble
