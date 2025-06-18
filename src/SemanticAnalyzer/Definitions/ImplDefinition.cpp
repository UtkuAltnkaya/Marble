#include "Ast/Definitions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{

    Ref<TypeSpecifier> ImplDefinition::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *node = table.Root();

        if (!m_ImplName->IsPrimitive())
        {
            node = HandleRoot(node);
        }

        table.EnterScope(node);
        for (auto &memberFunction : m_MemberFunctions)
        {
            memberFunction->Analyze(semanticAnalyzer);
            ;
        }
        table.LeaveScope();

        return TypeSpecifierOk;
    }

    Ref<TypeSpecifier> MemberFunctionDefinition::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        const SymbolNode *node = table.CurrentScope();
        SymbolNode *fnNode = node->Iter()
                                 .Function(m_Prototype->GetName())
                                 .Find();
        if (!fnNode)
        {
            throw "Cannot find function in this scope";
        }

        table.EnterScope(fnNode);
        m_Block->Analyze(semanticAnalyzer);
        ;
        table.LeaveScope();

        if (m_Prototype->GetReturnType()->GetType() == Types::Void)
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

    void ImplDefinition::CreateSymbol()
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *node = table.Root();

        if (!m_ImplName->IsPrimitive())
        {
            node = HandleRoot(node);
        }

        for (auto &memberFunction : m_MemberFunctions)
        {
            node->Insert(memberFunction->GetPrototype().GetName(), new FunctionSymbolNode{*memberFunction.get(), node});
        }
    }

    SymbolNode *ImplDefinition::HandleRoot(SymbolNode *node)
    {
        SymbolIterator iter = node->Iter();
        const std::string &name = m_ImplName->UserDefine().Id();

        if (auto symbolNode = iter.Struct(name).Find(); node)
        {
            return symbolNode;
        }
        if (auto symbolNode = iter.Reset().Enum(name).Find(); node)
        {
            return symbolNode;
        }
        throw "No such struct or enum";
    }
} // namespace Marble
