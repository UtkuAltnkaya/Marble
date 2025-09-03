#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include <llvm/Support/ManagedStatic.h>
namespace Marble
{
    CodegenContext::CodegenContext(const std::string &name)
        : m_Context{}, m_Builder{m_Context}, m_Module{MakeBox<llvm::Module>(name, m_Context)}
    {
    }

    CodegenContext::~CodegenContext()
    {
        llvm::llvm_shutdown();
    }

    void CodegenContext::Generate(Ref<Program> program)
    {
        for (auto &definition : program->Definitions())
        {
            definition->DeclareSignature(*this);
        }
        for (auto &definition : program->Definitions())
        {
            definition->Codegen(*this);
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

    StructOrEnumSymbolNode *CodegenContext::GetNamedUserDefinedType(const std::string &name)
    {
        return SymbolIterator().StructOrEnum(name);
    }
} // namespace Marble
