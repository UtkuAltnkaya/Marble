#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    llvm::Value *MemberAccessExpression::Codegen(CodegenContext &codegenContext)
    {
        switch (m_Object->ExpressionType())
        {
        case ExpressionType::NameSpace:
        case ExpressionType::FunctionCall:
        {
            TODO("TEMP IDENTIFIER");
            // llvm::Value *result = m_Object->Codegen(codegenContext);
            // IdentifierExpression *tempIdentifier = GetTempIdentifier();
            // if (!tempIdentifier)
            // {
            //     throw "Temp oject call must have temp variable";
            // }
            // llvm::Value *tempAddr = tempIdentifier->Address(codegenContext);
            // codegenContext.Builder().CreateStore(result, tempAddr);
            // break;
        }
        default:
            break;
        }

        if (m_Property->ExpressionType() == ExpressionType::FunctionCall)
        {
            return m_Property->Codegen(codegenContext);
        }

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

        return codegenContext.Builder().CreateLoad(elemType, addr);
    }

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
            objPtr = builder.CreateLoad(llvm::PointerType::get(objectType, 0), objPtr);
        }
        else
        {
            objectType = llvm::cast<llvm::StructType>(objectTypeSpecifier->ToLLVMType(codegenContext));
            identifier = &objectTypeSpecifier->UserDefine();
        }

        TODO("Find struct index");
        // SymbolTable &table = SymbolTable::GetInstance();
        // SymbolNode *node = table.Iter().Struct(identifier->Id()).Find();
        // StructDefinition *structDefinition = static_cast<StructDefinition *>(node->GetAstPtr());

        switch (m_Property->ExpressionType())
        {
        case ExpressionType::Identifier:
        {
            // auto identifier = m_Property->Into<IdentifierExpression>();
            // const std::string &fieldName = identifier->GetIdentifier().Id();
            // int index = structDefinition->GetFieldIndex(fieldName);
            // return builder.CreateStructGEP(objectType, objPtr, index);
        }
        case ExpressionType::ArrayIndex:
        {
            auto *arrayIndexExpr = m_Property->Into<ArrayIndexExpression>();
            // llvm::Value *arrayPtr = arrayIndexExpr-

            // // Step 2: Evaluate the index expression
            // llvm::Value *indexValue = arrayIndexExpr->IndexExpr()->Codegen(codegenContext);
        }
        case ExpressionType::FunctionCall:
        {
            ASSERT_D(false, "Must be unreachable");
            UNREACHABLE();
        }
        default:
            UNIMPLEMENTED("Member Access Expression");
            break;
        }
    }

} // namespace Marble
