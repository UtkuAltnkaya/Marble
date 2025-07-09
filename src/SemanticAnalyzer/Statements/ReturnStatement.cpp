#include "Ast/Statements.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    Ref<TypeSpecifier> ReturnStatement::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *parent = table.CurrentScope();

        while (parent && parent->GetSymbolData().NodeType() != SymbolNodeTypes::Function)
        {
            if (auto node = parent->Iter().Parent().Find(); node)
            {
                parent = node;
            }
            else
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find any parent of this symbol", true);
            }
        }

        FunctionSymbolNode *fnNode = parent->Into<FunctionSymbolNode>();
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
