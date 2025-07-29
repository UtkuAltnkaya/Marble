#include "Ast/Definitions.hpp"
#include "Codegen/Codegen.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    llvm::Value *StructDefinition::Codegen(CodegenContext &codegenContext)
    {
        llvm::LLVMContext &context = codegenContext.Context();
        llvm::Module &module = codegenContext.Module();
        llvm::IRBuilder<> &builder = codegenContext.Builder();

        const std::string &structName = m_StructName->Id();

        TODO("HANDLE STRUCT TYPE");
        // SymbolTable &table = SymbolTable::GetInstance();
        // SymbolNode *node = table.Iter().Struct(structName).Find();
        // ASSERT_D(node != nullptr, "Cannot find struct");
        // StructSymbolNode *structNode = node->Into<StructSymbolNode>();
        // llvm::StructType *structType = structNode->StructType();

        llvm::StructType *structType = nullptr;

        if (structType)
        {
            return nullptr;
        }

        if (!structType)
        {
            structType = llvm::StructType::create(context, "struct." + structName);
        }

        std::vector<llvm::Type *> fieldTypes;
        fieldTypes.reserve(m_Field.size());

        for (const auto &fieldDef : m_Field)
        {
            const auto &field = fieldDef->Into<StructFieldDefinition>();
            Ref<TypeSpecifier> fieldType = field->GetField().GetTypeSpecifier();
            llvm::Type *llvmType = fieldType->ToLLVMType(codegenContext);

            if (!llvmType)
            {
                throw "Failed to convert field type to LLVM type in struct " + structName;
            }
            fieldTypes.push_back(llvmType);
        }
        structType->setBody(fieldTypes);
        // structNode->StructType(structType);
        return nullptr;
    }

    int StructDefinition::GetFieldIndex(const std::string &fieldName)
    {
        for (size_t i = 0; i < m_Field.size(); i++)
        {
            const auto &field = m_Field.at(i);
            if (field->GetName() == fieldName)
            {
                return i;
            }
        }
        return -1;
    }

} // namespace Marble
