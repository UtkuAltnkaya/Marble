#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include <cstdint>

namespace Marble
{
    llvm::Value *NamespaceExpression::Codegen(CodegenContext &codegenContext)
    {
        if (m_Value->ExpressionType() == ExpressionType::FunctionCall)
        {
            return m_Value->Codegen(codegenContext);
        }

        SymbolTable &table = SymbolTable::GetInstance();
        IdentifierExpression *namespaceName = m_Namespace->Into<IdentifierExpression>();
        IdentifierExpression *valueName = m_Value->Into<IdentifierExpression>();

        if (auto node = table.Iter().Enum(namespaceName->GetIdentifier().Id()).Find())
        {
            EnumDefinition *enumDef = static_cast<Definition *>(node->GetAstPtr())->Into<EnumDefinition>();
            ASSERT_D(enumDef != nullptr, "Cannot get enum");

            int64_t constantValue = -1;
            const auto &constants = enumDef->GetFields();
            for (size_t i = 0; i < constants.size(); ++i)
            {
                if (*constants[i] == valueName->GetIdentifier())
                {
                    constantValue = static_cast<int64_t>(i);
                    break;
                }
            }
            llvm::LLVMContext &context = codegenContext.Context();
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), constantValue, true);
        }

        ASSERT_A(false, "Must be unreachable for now");
        UNREACHABLE();
    }
} // namespace Marble
