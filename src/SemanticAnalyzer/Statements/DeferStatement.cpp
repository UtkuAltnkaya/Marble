
#include "Ast/Statements.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    Ref<TypeSpecifier> DeferStatement::Analyze()
    {
        m_Expression->Analyze();
        return TypeSpecifierOk;
    }
} // namespace Marble
