#pragma once

#include <string>
#include <string_view>

namespace Marble
{
    class Ast;
    class SymbolNode;
    enum class SymbolNodeTypes;

    class SymbolIterator
    {
    public:
        SymbolIterator(SymbolNode *node);
        SymbolIterator(const SymbolNode *node);
        ~SymbolIterator() = default;

        SymbolNode *Find();
        SymbolIterator &Parent();
        SymbolIterator &Function(std::string_view name);
        SymbolIterator &Struct(std::string_view name);
        SymbolIterator &Enum(std::string_view name);
        SymbolIterator &Variable(std::string_view name);
        SymbolIterator &StructField(std::string_view name);
        size_t Count(SymbolNodeTypes filter);
        // TODO:SymbolIterator &EnumField(std::string_view name);

        template <typename T>
        SymbolIterator &Ok(T *AstNode)
        {
            static_assert(std::is_base_of<Ast, T>::value, "Type of paramater must be Ast node");
            if (!m_Flag)
            {
                return *this;
            }
            throw "Cannot find " + std::to_string((int)m_NodeType) + " node in this scope";
        }

    private:
        SymbolNode *const Find(std::string_view name, SymbolNodeTypes nodeType);
        inline void SetNode(SymbolNode *node) { m_Node = node; };

    private:
        SymbolNode *m_Node;
        SymbolNodeTypes m_NodeType;
        bool m_Flag;
    };

} // namespace Marble
