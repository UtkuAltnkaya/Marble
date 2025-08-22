#pragma once
#include "Utils/Derive/Concepts.hpp"

namespace Marble
{
    namespace Derive
    {
        class Printer
        {
        public:
            Printer() : m_Os{std::cout}, m_Indent{0} {}
            Printer(std::ostream &os) : m_Os{os}, m_Indent{0} {}
            ~Printer() = default;

            void Flush()
            {
                m_Os.flush();
            }

            void BeginObject(std::string_view className)
            {
                m_Os << className << " {\n";
                IncrementIndent();
            }

            void EndObject()
            {
                DecrementIndent();
                Indent();
                m_Os << "}\n";
            }

            template <typename T>
            void Field(std::string_view name, const T &value)
            {
                Indent();
                m_Os << name << ": ";
                Print(value);
                m_Os << "\n";
            }

            template <Streamable S>
                requires(!StringLike<S> && !PointerLike<S> && !Enumerable<S> && !Debuggable<S>)
            void Print(const S &stream)
            {
                m_Os << stream;
            }

            template <StringLike S>
            void Print(const S &str)
            {
                m_Os << '"' << str << '"';
            }

            template <PointerLike P>
            void Print(const P &ptr)
            {
                if (!ptr)
                {
                    m_Os << "nullptr";
                    return;
                }
                Print(*ptr);
            }

            template <Iterable I>
                requires(!StringLike<I>)
            void Print(const I &container)
            {
                m_Os << "[\n";
                IncrementIndent();
                for (auto &elem : container)
                {
                    Indent();
                    Print(elem);
                    m_Os << "\n";
                }
                DecrementIndent();
                Indent();
                m_Os << "]";
            }

            template <TupleLike T>
            void Print(const T &tuple)
            {
                m_Os << "(";
                IncrementIndent();
                PrintTupleImpl(tuple, std::make_index_sequence<std::tuple_size_v<T>>{});
                DecrementIndent();
                Indent();
                m_Os << ")";
            }

            template <typename... Ts>
            void Print(const std::variant<Ts...> &variant)
            {
                std::visit([this](const auto &alt)
                           {
                           using T = std::decay_t<decltype(alt)>;
                           if constexpr (!std::is_same_v<T, std::monostate>)
                               Print(alt); },
                           variant);
            }

            template <OptionalLike O>
            void Print(const O &opt)
            {
                if (!opt.has_value())
                {
                    m_Os << "nullopt";
                    return;
                }
                m_Os << "optional(";
                Print(*opt);
                m_Os << ")";
            }

            template <Enumerable E>
            void Print(const E &enumEl)
            {
                using U = std::underlying_type_t<E>;
                m_Os << "enum(" << static_cast<U>(enumEl) << ")";
            }

            template <Debuggable D>
            void Print(const D &obj)
            {
                obj.Debuggable(*this);
            }

        private:
            inline void Indent() { m_Os << std::string(m_Indent * 3, ' '); }
            inline void IncrementIndent() { m_Indent++; }
            inline void DecrementIndent() { m_Indent--; }

            template <TupleLike T, std::size_t... Is>
            void PrintTupleImpl(const T &tuple, std::index_sequence<Is...>)
            {
                Indent();
                bool first = true;
                ((first ? (first = false, Print(std::get<Is>(tuple)))
                        : (m_Os << ", ", Print(std::get<Is>(tuple)))),
                 ...);
            }

        private:
            std::ostream &m_Os;
            int m_Indent;
        };
    }

} // namespace Marble
