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
            auto &ptr = std::get<PointerType>(m_Variants);
            return llvm::PointerType::getUnqual(ptr.TypeSpecifier->ToLLVMType(codegenContext));
        }
        case Types::ArrayType:
        {
            auto &arr = std::get<ArrayType>(m_Variants);
            return llvm::ArrayType::get(arr.TypeSpecifier->ToLLVMType(codegenContext), arr.Size);
        }
        case Types::UserDefine:
        {
            const UserDefineType &id = std::get<UserDefineType>(m_Variants);
            SymbolNode *node = codegenContext.GetNamedUserDefinedType(*id.Type);
            if (node->GetSymbolData().NodeType() == SymbolNodeTypes::Struct)
            {
                TODO("HANDLE STRUCT TYPE");
                // StructSymbolNode *structSymbol = node->Into<StructSymbolNode>();
                // llvm::StructType *structType = structSymbol->StructType();
                // if (!structType)
                // {
                //     structSymbol->GetAstPtr()->Codegen(codegenContext);
                //     structType = structSymbol->StructType();
                //     ASSERT_D(structType != nullptr, "Cannot declare struct");
                // }
                // return structType;
            }
            return llvm::Type::getInt32Ty(context);
        }
        case Types::ConstantType:
        {
            auto &constType = std::get<ConstantType>(m_Variants);
            return constType.TypeSpecifier->ToLLVMType(codegenContext);
        }
        case Types::GenericType:
            ASSERT_A(false, "Generics must be expanded at this stage");
        default:
            UNREACHABLE();
            break;
        }
    }
} // namespace Marble
