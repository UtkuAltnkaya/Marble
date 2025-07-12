#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    llvm::Value *AssignmentExpression::Codegen(CodegenContext &codegenContext)
    {
        llvm::IRBuilder<> &builder = codegenContext.Builder();

        llvm::Value *value = m_Value->Codegen(codegenContext);
        if (!value)
        {
            throw "Invalid right-hand side expression";
        }
        llvm::Value *address = m_Variable->Address(codegenContext);
        if (!address)
        {
            throw "Invalid left-hand side expression - cannot get address";
        }
        builder.CreateStore(value, address);
        return value;
    }

} // namespace Marble
