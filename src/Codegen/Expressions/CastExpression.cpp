#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"

namespace Marble
{
    enum class ConversionKind
    {
        None,
        Identity,
        WideningNumeric,
        NarrowingNumeric,
        ConstConversion,
        Invalid
    };

    std::string TypeToString(llvm::Type *type)
    {
        std::string result;
        llvm::raw_string_ostream rso(result);
        type->print(rso);
        return rso.str();
    }

    llvm::Value *CastExpression::Codegen(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::Value *value = m_Expression->Codegen(codegenContext);

        if (!value)
        {
            throw "Failed to evaluate expression in cast";
        }

        llvm::Type *targetType = m_TypeSpecifier->ToLLVMType(codegenContext);
        llvm::Type *sourceType = value->getType();

        if (targetType == sourceType || m_Kind == ConversionKind::Identity || m_Kind == ConversionKind::ConstConversion)
        {
            return value;
        }

        switch (m_Kind)
        {
        case ConversionKind::WideningNumeric:
        {
            if (sourceType->isIntegerTy() && targetType->isFloatingPointTy())
                return builder.CreateSIToFP(value, targetType, "sitofp");
            if (sourceType->isIntegerTy() && targetType->isIntegerTy() && targetType->getIntegerBitWidth() > sourceType->getIntegerBitWidth())
                return builder.CreateSExt(value, targetType, "sext");
            if (sourceType->isFloatingPointTy() && targetType->isFloatingPointTy() && targetType->getPrimitiveSizeInBits() > sourceType->getPrimitiveSizeInBits())
                return builder.CreateFPExt(value, targetType, "fpext");
            break;
        }
        case ConversionKind::NarrowingNumeric:
        {
            if (sourceType->isFloatingPointTy() && targetType->isIntegerTy())
                return builder.CreateFPToSI(value, targetType, "fptosi");
            if (sourceType->isIntegerTy() && targetType->isIntegerTy() && targetType->getIntegerBitWidth() < sourceType->getIntegerBitWidth())
                return builder.CreateTrunc(value, targetType, "trunc");
            if (sourceType->isFloatingPointTy() && targetType->isFloatingPointTy() && targetType->getPrimitiveSizeInBits() < sourceType->getPrimitiveSizeInBits())
                return builder.CreateFPTrunc(value, targetType, "fptrunc");
            break;
        }
        case ConversionKind::Invalid:
        case ConversionKind::None:
        default:
            break;
        }

        throw "Invalid cast from " + TypeToString(sourceType) + " to " + TypeToString(targetType);
    }
} // namespace Marble
