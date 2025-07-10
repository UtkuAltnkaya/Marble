#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"
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

    SymbolNode *CodegenContext::GetNamedUserDefinedType(const std::string &name)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolIterator iter = table.Iter();
        if (auto node = iter.Struct(name).Find(); node)
        {
            return node;
        }
        if (auto node = iter.Reset().Enum(name).Find(); node)
        {
            return node;
        }
        return nullptr;
    }
} // namespace Marble
