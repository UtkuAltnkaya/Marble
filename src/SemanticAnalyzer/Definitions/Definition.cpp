#include "Ast/Definitions.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    void Definition::CheckParametersType(SemanticAnalyzer &analyzer, Ref<TypeSpecifier> paramType)
    {
        switch (paramType->GetType())
        {
        case Types::UserDefine:
        {
            SymbolIterator iter;

            if (auto node = iter.Struct(paramType->ToString()); node)
            {
                return;
            }
            if (auto node = iter.Enum(paramType->ToString()); node)
            {
                return;
            }
            ErrorSystem::AddError(analyzer, this, "Cannot find the type named " + paramType->ToString(), false);
            break;
        }
        case Types::Pointer:
        {
            auto &ptr = paramType->Pointer();
            return CheckParametersType(analyzer, ptr.TypeSpecifier);
        }
        case Types::ArrayType:
        {
            auto &arr = paramType->Array();
            return CheckParametersType(analyzer, arr.TypeSpecifier);
        }
        case Types::GenericType:
        {
            ASSERT_A(false, "Generics must be expanded");
            break;
        }
        default:
            break;
        }
    }

} // namespace Marble
