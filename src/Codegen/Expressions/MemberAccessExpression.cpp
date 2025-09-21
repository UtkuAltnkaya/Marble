#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    llvm::Value *MemberAccessExpression::Codegen(CodegenContext &codegenContext)
    {
        llvm::Value *baseAddress = m_Object->Address(codegenContext);
    }

    llvm::Value *MemberAccessExpression::Address(CodegenContext &codegenContext)
    {
    }

} // namespace Marble
