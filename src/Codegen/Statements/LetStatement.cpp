#include "Ast/Statements.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    llvm::Value *LetStatement::Codegen(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::Value *value = nullptr;
        if (m_Value)
        {
            value = m_Value->Codegen(codegenContext);
        }

        llvm::Type *llvmType = nullptr;
        if (m_TypeSpecifier)
        {
            llvmType = m_TypeSpecifier->ToLLVMType(codegenContext);
        }
        else
        {
            llvmType = value->getType();
        }

        llvm::Function *currentFunction = builder.GetInsertBlock()->getParent();
        llvm::AllocaInst *alloca = codegenContext.CreateEntryBlockAlloca(currentFunction, llvmType, m_Identifier->Id());

        if (value)
        {
            builder.CreateStore(value, alloca);
        }

        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *node = table.CurrentScope()->Iter().Variable(m_Identifier->Id()).Find();
        ASSERT_D(node != nullptr, "Variable not found in the scope");
        VariableSymbolNode *variableNode = node->Into<VariableSymbolNode>();
        variableNode->SetAlloca(alloca);

        return nullptr;
    }

} // namespace Marble
