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
        SymbolIterator &EnumField(std::string_view name);
        size_t Count(SymbolNodeTypes filter);
        inline bool IsFound() const { return !m_Flag; }
        SymbolIterator &Reset();

    private:
        SymbolNode *const Find(std::string_view name, SymbolNodeTypes nodeType);
        inline void SetNode(SymbolNode *node) { m_Node = node; };

    private:
        SymbolNode *m_Node;
        SymbolNode *m_StartNode;
        SymbolNodeTypes m_NodeType;
        bool m_Flag;
    };

} // namespace Marble
