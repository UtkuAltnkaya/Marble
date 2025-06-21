#pragma once

#include <vector>
#include "Ast/Ast.hpp"
#include "Ast/TypeSpecifier.hpp"

namespace Marble
{

    class Generics : public Ast
    {
    public:
        Generics(std::vector<Ref<TypeSpecifier>> &&types, const Span &span);
        Generics(const Generics &obj);
        ~Generics() = default;

        static Box<Generics> Parse(Parser &parser);
        inline const std::vector<Ref<TypeSpecifier>> &Types() const { return m_Types; }
        std::unordered_map<std::string, Ref<TypeSpecifier>> ToMap(const std::vector<Ref<TypeSpecifier>> &typeArgs);

        bool operator==(const Generics &obj) const
        {
            if (m_Types.size() != obj.m_Types.size())
            {
                return false;
            }
            for (size_t i = 0; i < m_Types.size(); i++)
            {
                if (m_Types[i]->operator!=(*obj.m_Types[i]))
                {
                    return false;
                }
            }
            return true;
        }

    private:
        std::vector<Ref<TypeSpecifier>> m_Types;
    };
    struct GenericInstanceKey
    {
        std::string Name;
        std::vector<std::string> TypeArgumentNames;

        bool operator==(const GenericInstanceKey &obj) const
        {
            if (Name != obj.Name)
            {
                return false;
            }

            if (TypeArgumentNames.size() != obj.TypeArgumentNames.size())
            {
                return false;
            }
            for (size_t i = 0; i < TypeArgumentNames.size(); i++)
            {
                if (TypeArgumentNames[i] != obj.TypeArgumentNames[i])
                {
                    return false;
                }
            }
            return true;
        }
    };

    struct GenericInstanceKeyHasher
    {
        std::size_t operator()(const GenericInstanceKey &key) const
        {
            std::size_t h = std::hash<std::string>{}(key.Name);
            for (const auto &arg : key.TypeArgumentNames)
            {
                h ^= std::hash<std::string>{}(arg) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };

} // namespace Marble
