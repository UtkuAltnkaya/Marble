#include "Ast/TypeSpecifier.hpp"
#include "Codegen/Codegen.hpp"
#include "Utils/Macros.hpp"

namespace Marble
{
    llvm::Type *TypeSpecifier::ToLLVMType(CodegenContext &codegenContext)
    {
        llvm::LLVMContext &context = codegenContext.Context();
        switch (m_Type)
        {
        case Types::Int:
            return llvm::Type::getInt32Ty(context);
        case Types::Usize:
            return llvm::Type::getInt64Ty(context);
        case Types::Float:
            return llvm::Type::getFloatTy(context);
        case Types::Double:
            return llvm::Type::getDoubleTy(context);
        case Types::Char:
            return llvm::Type::getInt8Ty(context);
        case Types::Str:
            return llvm::PointerType::getUnqual(context);
        case Types::Bool:
            return llvm::Type::getInt1Ty(context);
        case Types::Void:
            return llvm::Type::getVoidTy(context);
        case Types::Null:
            return llvm::PointerType::getUnqual(context);
        case Types::GenericType:
            ASSERT_A(false, "Generics must be expanded at this stage");
        default:
            UNIMPLEMENTED("");
            break;
        }
    }
} // namespace Marble
