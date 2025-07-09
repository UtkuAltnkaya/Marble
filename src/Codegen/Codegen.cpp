#include "Codegen/Codegen.hpp"

namespace Marble
{
    CodegenContext::CodegenContext(const std::string &name)
        : m_Context{}, m_Builder{m_Context}, m_Module{MakeBox<llvm::Module>(name, m_Context)}
    {
    }

    void CodegenContext::Generate(Ref<Program> program)
    {
        for (auto &definition : program->Definitions())
        {
            if (!definition->IsGeneric())
            {
                definition->Codegen(*this);
            }
        }
    }

    void CodegenContext::Print()
    {
        m_Module->print(llvm::outs(), nullptr);
    }

    llvm::AllocaInst *CodegenContext::CreateEntryBlockAlloca(llvm::Function *function, llvm::Type *type, const std::string &name)
    {
        llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
        return tmpB.CreateAlloca(type, nullptr, name);
    }

} // namespace Marble
