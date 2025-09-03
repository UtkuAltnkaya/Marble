#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include "Utils/Memory.hpp"
#include "Ast/Program.hpp"

namespace Marble
{
    class SymbolNode;
    class CodegenContext
    {
    public:
        CodegenContext(const std::string &name);
        ~CodegenContext();

        void Generate(Ref<Program> program);
        void Print();

        llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *function, llvm::Type *type, const std::string &name = "");
        StructOrEnumSymbolNode *GetNamedUserDefinedType(const std::string &name);

        inline llvm::LLVMContext &Context() { return m_Context; }
        inline llvm::IRBuilder<> &Builder() { return m_Builder; }
        inline llvm::Module &Module() { return *m_Module; }

    private:
        llvm::LLVMContext m_Context;
        llvm::IRBuilder<> m_Builder;
        Box<llvm::Module> m_Module;
    };

} // namespace Marble
