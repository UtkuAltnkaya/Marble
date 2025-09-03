#include "Ast/TypeSpecifier.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"
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
        case Types::Pointer:
        {
            return llvm::PointerType::getUnqual(PointerUnchecked().TypeSpecifier->ToLLVMType(codegenContext));
        }
        case Types::ArrayType:
        {
            auto &arr = ArrayUnchecked();
            return llvm::ArrayType::get(arr.TypeSpecifier->ToLLVMType(codegenContext), arr.Size);
        }
        case Types::UserDefine:
        {
            return ToUserDefineType(codegenContext);
        }
        case Types::ConstantType:
        {
            return ConstantUnchecked().TypeSpecifier->ToLLVMType(codegenContext);
        }
        case Types::GenericType:
            ASSERT_A(false, "Generics must be expanded at this stage");
        default:
            UNREACHABLE();
            break;
        }
    }

    llvm::Type *TypeSpecifier::ToUserDefineType(CodegenContext &codegenContext)
    {
        llvm::LLVMContext &context = codegenContext.Context();

        const UserDefineType &id = UserDefineUnchecked();
        StructOrEnumSymbolNode *node = codegenContext.GetNamedUserDefinedType(*id.Type);

        if (node->GetSymbolData().NodeType() == SymbolNodeTypes::Enum)
        {
            return llvm::Type::getInt32Ty(context);
        }

        llvm::StructType *structType = node->LLVMStructType();
        if (structType)
        {
            return structType;
        }

        StructDefinition *ast = node->Ast<StructDefinition>();
        ast->Codegen(codegenContext);
        structType = node->LLVMStructType();
        ASSERT_D(structType != nullptr, "Cannot declare struct");
        return structType;
    }
} // namespace Marble
