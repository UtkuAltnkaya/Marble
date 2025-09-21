#include "Ast/Statements.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    llvm::Value *LetStatement::Codegen(CodegenContext &codegenContext)
    {
        ASSERT_D(m_TypeSpecifier != nullptr, "Type must be referred at semantic analysis.");

        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::Function *currentFunction = builder.GetInsertBlock()->getParent();
        llvm::Type *llvmType = m_TypeSpecifier->ToLLVMType(codegenContext);
        llvm::AllocaInst *alloca = codegenContext.CreateEntryBlockAlloca(currentFunction, llvmType);

        SymbolNode *currentScope = SymbolTable::Get().CurrentScope();
        VariableSymbolNode *variableNode = SymbolIterator(currentScope).Variable(**m_Identifier);
        ASSERT_D(variableNode != nullptr, "Variable not found in the scope");
        variableNode->Alloca(alloca);

        if (!m_Value)
        {
            return nullptr;
        }

        switch (m_Value->ExpressionType())
        {
        case ExpressionType::ArrayInit:
        {
            auto expr = m_Value->Into<ArrayInitExpression>();
            expr->CodegenInPlace(codegenContext, alloca);
            break;
        }
        case ExpressionType::ObjectInit:
        {
            auto expr = m_Value->Into<ObjectInitExpression>();
            expr->CodegenInPlace(codegenContext, alloca);
            break;
        }
        default:
            llvm::Value *value = m_Value->Codegen(codegenContext);
            builder.CreateStore(value, alloca);
        }

        return nullptr;
    }

} // namespace Marble
