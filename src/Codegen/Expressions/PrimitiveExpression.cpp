#include "Ast/Expressions.hpp"
#include "Codegen/Codegen.hpp"

namespace Marble
{

    llvm::Value *PrimitiveExpression::Codegen(CodegenContext &codegenContext)
    {
        llvm::LLVMContext &context = codegenContext.Context();

        switch (m_TypeSpecifier->GetType())
        {
        case Types::Int:
        {
            int value = std::stoi(m_Value);
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), value, true);
        }
        case Types::Float:
        {
            float value = std::stof(m_Value);
            return llvm::ConstantFP::get(llvm::Type::getFloatTy(context), m_Value);
        }
        case Types::Bool:
        {
            bool value = (m_Value == "true");
            return llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), value);
        }
        case Types::Null:
        {
            auto *ptr = llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0);
            return llvm::ConstantPointerNull::get(ptr);
        }
        case Types::Char:
        {
            if (m_Value.size() == 1)
            {
                char ch = m_Value[0];
                return llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), static_cast<uint8_t>(ch));
            }
            else if (m_Value.size() >= 2 && m_Value[0] == '\\')
            {
                UNIMPLEMENTED("Escape sequence");
            }
        }
        case Types::Str:
        {
            return codegenContext.Builder().CreateGlobalString(m_Value, ".str");
        }
        default:
            throw "Unknown primitive kind";
        }
    }
} // namespace Marble
