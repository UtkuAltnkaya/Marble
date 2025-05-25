#include "Ast/Definitions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"

namespace Marble
{

  FunctionDefinition::FunctionDefinition(AccessSpecifier accessSpecifier, Box<Identifier> functionName, Box<Generics> generics,
                                         std::vector<Box<VariableType>> &&params, Ref<TypeSpecifier> returnType,
                                         Box<Statement> block, const Span &span)
      : Definition{span, DefinitionType::Function}, m_AccessSpecifier{accessSpecifier}, m_FunctionName{std::move(functionName)},
        m_Generics{std::move(generics)}, m_Params{std::move(params)}, m_ReturnType{std::move(returnType)}, m_Block{std::move(block)}

  {
  }

  Box<Definition> FunctionDefinition::Parse(Parser &parser, AccessSpecifier accessSpecifier, const Span &span)
  {
    Span start = accessSpecifier == AccessSpecifier::Public ? span : parser.Current().Span();
    // Function Name
    Box<Identifier> functionName = Identifier::Parse(parser);

    Box<Generics> generics = Generics::Parse(parser);

    // Parameters
    std::vector<Box<VariableType>> params;
    parser.NextTokenAndExpect(TokenType::OpenParen);
    Parenthesis::Parse<Box<VariableType>>(params, parser, TokenType::CloseParen, [](Parser &parser)
                                          { return VariableType::Parse(parser); });

    // Return Type
    parser.NextTokenAndExpect(TokenType::Arrow);
    parser.NextToken();
    Ref<TypeSpecifier> returnType = TypeSpecifier::Parse(parser);

    // Block
    Box<Statement> block = BlockStatement::Parse(parser);

    const Span &end = parser.Current().Span();

    Box<FunctionDefinition> fnDefinition = MakeBox<FunctionDefinition>(
        accessSpecifier, std::move(functionName), std::move(generics),
        std::move(params), returnType, std::move(block), Span{start.Start, end.Start});

    // TODO: Insert Symbol

    return fnDefinition;
  }
} // namespace Marble
