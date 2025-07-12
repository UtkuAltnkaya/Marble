#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"

namespace Marble
{

    llvm::Value *ExpressionStatement::Codegen(CodegenContext &codegenContext)
    {
        return m_Expression->Codegen(codegenContext);
    }
} // namespace Marble
