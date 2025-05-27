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
        SymbolIterator(const SymbolNode *node);
        ~SymbolIterator() = default;

        const SymbolNode *const Find();
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
        const SymbolNode *const Find(std::string_view name, SymbolNodeTypes nodeType);
        inline void SetNode(SymbolNode *node);
        inline void SetNode(const SymbolNode *node);

    private:
        const SymbolNode *m_Node;
        SymbolNodeTypes m_NodeType;
        bool m_Flag;
    };

} // namespace Marble
