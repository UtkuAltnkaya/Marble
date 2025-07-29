#pragma once

#include <string>
#include <string_view>
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    class Ast;
    class SymbolNode;
    class BlockSymbolNode;
    class FunctionSymbolNode;
    class VariableSymbolNode;
    class StructSymbolNode;
    class EnumSymbolNode;
    enum class SymbolNodeTypes;

    class SymbolIterator
    {
    public:
        SymbolIterator();
        SymbolIterator(SymbolNode *node);
        ~SymbolIterator() = default;

        FunctionSymbolNode *Function(const std::string &name);
        StructSymbolNode *Struct(const std::string &name);
        VariableSymbolNode *Variable(const std::string &name);
        EnumSymbolNode *Enum(const std::string &name);

    private:
        VariableSymbolNode *Variable(BlockSymbolNode *blockNode, const std::string &name);
        SymbolNode *Find(BlockSymbolNode *node, const std::string &name, SymbolNodeTypes type);

    private:
        BlockSymbolNode *m_Node;
    };

} // namespace Marble
