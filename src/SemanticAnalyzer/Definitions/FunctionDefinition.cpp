#include "Ast/Definitions.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include <iostream>

namespace Marble
{
    Ref<TypeSpecifier> FunctionDefinition::Analyze()
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *iter = table.Iter()
                               .Function(m_FunctionName->Id())
                               .Ok<FunctionDefinition>(this)
                               .Find();

        table.EnterScope(iter);
        m_Block->Analyze();
        table.LeaveScope();

        if (m_ReturnType->GetType() == Types::Void)
        {
            return TypeSpecifierOk;
        }

        BlockStatement *block = static_cast<BlockStatement *>(m_Block.get());

        const std::vector<Box<Statement>> &statements = block->Statements();
        if (statements.size() == 0)
        {
            // TODO: Decide to allow empty function body or not, or warn
        }

        if (statements.back()->StatementType() == StatementType::Return)
        {
            return TypeSpecifierOk;
        }

        throw "Return statement expected";
    }

    bool FunctionDefinition::operator==(const FunctionDefinition &obj) const
    {
        if (m_AccessSpecifier != obj.m_AccessSpecifier)
        {
            return false;
        }
        if (*m_FunctionName != *obj.m_FunctionName)
        {
            return false;
        }
        if (m_Generics && obj.m_Generics && *m_Generics != *obj.m_Generics)
        {
            return false;
        }
        if (m_Params.size() != obj.m_Params.size())
        {
            return false;
        }
        for (size_t i = 0; i < m_Params.size(); i++)
        {
            if (*m_Params[i] != *obj.m_Params[i])
            {
                return false;
            }
        }
        if (*m_ReturnType != *obj.m_ReturnType)
        {
            return false;
        }
        // TODO compare block
        return true;
    }
} // namespace Marble
