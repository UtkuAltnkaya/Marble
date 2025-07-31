#include "Ast/Statements.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ReturnStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::Get();
        FunctionSymbolNode *fnNode = SymbolIterator(table.CurrentScope()).Function();
        if (!fnNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find function which return statement used into.", true);
        }

        Ref<TypeSpecifier> returnType = fnNode->ReturnType();

        // TODO: Warn if local addresses returns
        if (m_Expression)
        {
            Ref<TypeSpecifier> valueType = m_Expression->Analyze(semanticAnalyzer);
            if (*valueType == *returnType)
            {
                return TypeSpecifierOk;
            }
            if (semanticAnalyzer.TryImplicitConversion(m_Expression, valueType, returnType))
            {
                return TypeSpecifierOk;
            }
        }
        else if (returnType->GetType() == Types::Void)
        {
            return TypeSpecifierOk;
        }
        ErrorSystem::AddError(semanticAnalyzer, this, "Return value and return type of function does not match");
        return TypeSpecifierOk;
    }

    void ReturnStatement::SubstituteGenerics(
        SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        if (m_Expression)
        {
            m_Expression->SubstituteGenerics(semanticAnalyzer, map);
        }
    }

    Box<Statement> ReturnStatement::Clone()
    {
        return MakeBox<ReturnStatement>(*this);
    }
} // namespace Marble
