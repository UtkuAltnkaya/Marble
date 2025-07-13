#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    llvm::Value *MemberAccessExpression::Codegen(CodegenContext &codegenContext)
    {
        llvm::Value *addr = Address(codegenContext);
        if (!addr)
        {
            throw "Cannot find the address of object";
        }

        llvm::Type *elemType = m_ValueType->ToLLVMType(codegenContext);
        if (elemType->isFunctionTy() || elemType->isPointerTy())
        {
            return addr;
        }

        return codegenContext.Builder().CreateLoad(elemType, addr, "member_load");
    }

    // TODO
    llvm::Value *MemberAccessExpression::Address(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::LLVMContext &context = codegenContext.Context();

        llvm::Value *objPtr = m_Object->Address(codegenContext);
        if (!objPtr)
        {
            throw "Failed to get address of base object in member access";
        }

        Ref<TypeSpecifier> objectTypeSpecifier = m_Object->ValueType();
        llvm::StructType *objectType = nullptr;
        const Identifier *identifier = nullptr;

        if (objectTypeSpecifier->GetType() == Types::Pointer)
        {
            auto &ptr = objectTypeSpecifier->Pointer();
            objectType = llvm::cast<llvm::StructType>(ptr.TypeSpecifier->ToLLVMType(codegenContext));
            identifier = &ptr.TypeSpecifier->UserDefine();
        }
        else
        {
            objectType = llvm::cast<llvm::StructType>(objectTypeSpecifier->ToLLVMType(codegenContext));
            identifier = &objectTypeSpecifier->UserDefine();
        }

        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *node = table.Iter().Struct(identifier->Id()).Find();
        StructDefinition *structDefinition = static_cast<StructDefinition *>(node->GetAstPtr());

        switch (m_Property->ExpressionType())
        {
        case ExpressionType::Identifier:
        {
            auto identifier = m_Property->Into<IdentifierExpression>();
            const std::string &fieldName = identifier->GetIdentifier().Id();
            int index = structDefinition->GetFieldIndex(fieldName);
            return builder.CreateStructGEP(objectType, objPtr, index);
        }
        default:
            UNIMPLEMENTED("Member Access Expression");
            break;
        }
    }
} // namespace Marble
