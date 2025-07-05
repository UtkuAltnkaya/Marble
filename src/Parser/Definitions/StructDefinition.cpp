#include "Ast/Definitions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{

    StructDefinition::StructDefinition(
        AccessSpecifier accessSpecifier, Box<Identifier> structName, Box<Generics> generics, std::vector<Box<StructFieldDefinition>> &&field, const Span &span)
        : Definition{span, DefinitionType::Struct},
          m_AccessSpecifier{accessSpecifier}, m_StructName{std::move(structName)},
          m_Generics{std::move(generics)}, m_Field{std::move(field)}, m_ImplDefinition{nullptr}

    {
    }

    StructDefinition::StructDefinition(const StructDefinition &obj) : Definition{obj.m_Span, DefinitionType::Struct}
    {
        m_AccessSpecifier = obj.m_AccessSpecifier;
        m_StructName = MakeBox<Identifier>(*obj.m_StructName.get());
        m_Generics = obj.m_Generics ? MakeBox<Generics>(*obj.m_Generics.get()) : nullptr;
        m_ImplDefinition = obj.m_ImplDefinition;
        for (auto &field : obj.m_Field)
        {
            m_Field.push_back(MakeBox<StructFieldDefinition>(*field.get()));
        }
    }

    Box<Definition> StructDefinition::Parse(Parser &parser, AccessSpecifier accessSpecifier, const Span &span)
    {
        Span start = accessSpecifier == AccessSpecifier::Public ? span : parser.Current().Span();

        // Struct Name
        Box<Identifier> structName = Identifier::Parse(parser);
        Box<Generics> generics = Generics::Parse(parser);

        parser.NextTokenAndExpect(TokenType::OpenCurlyBrace);

        // Struct Fields
        std::vector<Box<StructFieldDefinition>> fields;
        Parenthesis::Parse<Box<StructFieldDefinition>>(fields, parser, TokenType::CloseCurlyBrace, [](Parser &parser)
                                                       { return StructFieldDefinition::Parse(parser); });

        const Span &end = parser.Current().Span();
        Box<StructDefinition> structDefinition = MakeBox<StructDefinition>(
            accessSpecifier, std::move(structName), std::move(generics), std::move(fields), Span{start.Start, end.End});

        SymbolTable &table = SymbolTable::GetInstance();
        table.Insert(structDefinition->m_StructName->Id(), new SymbolNode{*structDefinition.get(), table.Root()});
        return structDefinition;
    }

    StructFieldDefinition::StructFieldDefinition(AccessSpecifier accessSpecifier, Box<VariableType> field, const Span &span)
        : Definition{span, DefinitionType::StructField}, m_AccessSpecifier{accessSpecifier}, m_Field{std::move(field)}
    {
    }

    StructFieldDefinition::StructFieldDefinition(const StructFieldDefinition &obj)
        : Definition{obj.m_Span, DefinitionType::StructField}
    {
        m_AccessSpecifier = obj.m_AccessSpecifier;
        m_Field = MakeBox<VariableType>(*obj.m_Field.get());
    }

    Box<StructFieldDefinition> StructFieldDefinition::Parse(Parser &parser)
    {
        Span span;
        AccessSpecifier accessSpecifier = ParseAccessSpecifier(parser, span);

        Span start = accessSpecifier == AccessSpecifier::Public ? span : parser.Current().Span();
        Box<VariableType> field = VariableType::Parse(parser);
        const Span &end = parser.Current().Span();

        return MakeBox<StructFieldDefinition>(accessSpecifier, std::move(field), Span{start.Start, end.Start});
    }
} // namespace Marble
