#include "Ast/Definitions.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{

    llvm::Value *FunctionDefinition::DeclareSignature(CodegenContext &codegenContext)
    {
        std::vector<llvm::Type *> paramTypes;
        paramTypes.reserve(m_Params.size());
        for (auto &param : m_Params)
        {
            llvm::Type *type = param->GetTypeSpecifier()->ToLLVMType(codegenContext);
            paramTypes.push_back(type);
        }

        llvm::Type *returnType = m_ReturnType->ToLLVMType(codegenContext);
        llvm::FunctionType *functionType = llvm::FunctionType::get(returnType, paramTypes, false);

        llvm::Function *function = llvm::Function::Create(
            functionType,
            llvm::Function::ExternalLinkage,
            GetName(),
            codegenContext.Module());

        unsigned i = 0;
        for (auto &arg : function->args())
        {
            const auto &param = m_Params[i];
            arg.setName(m_Params[i++]->GetIdentifier().Id());
        }
        return function;
    }

    // TODO handle function arguments, allocate or not;
    llvm::Value *FunctionDefinition::Codegen(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();
        llvm::Function *function = codegenContext.Module().getFunction(GetName());

        if (!function)
        {
            throw "Function declaration must exist before codegen body";
        }

        llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(codegenContext.Context(), "entry", function);
        builder.SetInsertPoint(entryBB);

        SymbolTable &table = SymbolTable::Get();
        SymbolNode *node = SymbolIterator().Function(GetName());
        SymbolIterator fnIter(node);

        if (!node)
        {
            throw "Cannot find function";
        }

        table.EnterScope(node);
        unsigned i = 0;
        for (auto &arg : function->args())
        {
            const auto &param = m_Params.at(i);

            VariableSymbolNode *paramNode = fnIter.Variable(param->GetIdentifier().Id());
            ASSERT_D(paramNode != nullptr, "Cannot find parameter in this scope");

            Ref<TypeSpecifier> paramType = param->GetTypeSpecifier();

            if (true && paramType->GetType() != Types::ConstantType)
            {
                llvm::AllocaInst *alloca = codegenContext.CreateEntryBlockAlloca(function, arg.getType());
                builder.CreateStore(&arg, alloca);
                // paramVariable->SetAlloca(alloca);
            }
            i++;
        }

        m_Block->Codegen(codegenContext);

        table.LeaveScope();

        BlockStatement *block = m_Block->Into<BlockStatement>();
        const std::vector<Box<Statement>> &statements = block->Statements();
        if (statements.size() == 0)
        {
            builder.CreateRetVoid();
        }
        else
        {
            const Box<Statement> &lastStatement = block->Statements().back();
            if (lastStatement->StatementType() != StatementType::Return)
            {
                builder.CreateRetVoid();
            }
        }
        return function;
    }
} // namespace Marble
