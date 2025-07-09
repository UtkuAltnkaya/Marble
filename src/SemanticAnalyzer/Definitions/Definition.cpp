#include "Ast/Definitions.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    void Definition::CheckParametersType(SemanticAnalyzer &analyzer, Ref<TypeSpecifier> paramType, SymbolIterator &iter)
    {
        switch (paramType->GetType())
        {
        case Types::UserDefine:
        {
            if (auto node = iter.Reset().Struct(paramType->ToString()).Find(); node)
            {
                return;
            }
            if (auto node = iter.Reset().Enum(paramType->ToString()).Find(); node)
            {
                return;
            }
            ErrorSystem::AddError(analyzer, this, "Cannot find the type named " + paramType->ToString(), false);
            break;
        }
        case Types::Pointer:
        {
            auto &ptr = paramType->Pointer();
            return CheckParametersType(analyzer, ptr.TypeSpecifier, iter);
        }
        case Types::ArrayType:
        {
            auto &arr = paramType->Array();
            return CheckParametersType(analyzer, arr.TypeSpecifier, iter);
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
