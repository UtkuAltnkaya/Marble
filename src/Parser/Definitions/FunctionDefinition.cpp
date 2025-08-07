#include "Ast/Definitions.hpp"
#include "Parser/Parser.hpp"
#include "Parser/Parenthesis.hpp"
#include "Utils/IDGenerator.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{

  FunctionDefinition::FunctionDefinition(AccessSpecifier accessSpecifier, Box<Identifier> functionName, Box<Generics> generics,
                                         std::vector<Box<VariableType>> &&params, Ref<TypeSpecifier> returnType,
                                         Box<Statement> block, const Span &span)
      : Definition{span, DefinitionType::Function}, m_AccessSpecifier{accessSpecifier}, m_FunctionName{std::move(functionName)},
        m_Generics{std::move(generics)}, m_Params{std::move(params)}, m_ReturnType{returnType}, m_Block{std::move(block)}, m_IsMethod{false}

  {
  }

  FunctionDefinition::FunctionDefinition(const FunctionDefinition &obj) : Definition{obj.m_Span, DefinitionType::Function}
  {
    m_AccessSpecifier = obj.m_AccessSpecifier;
    m_FunctionName = MakeBox<Identifier>(*obj.m_FunctionName.get());
    m_Generics = obj.m_Generics ? MakeBox<Generics>(*obj.m_Generics.get()) : nullptr;
    m_IsMethod = obj.m_IsMethod;
    for (auto &param : obj.m_Params)
    {
      m_Params.push_back(param->Clone());
    }
    m_ReturnType = obj.m_ReturnType->Clone();
    m_Block = obj.m_Block->Clone();
  }

  FunctionDefinition::FunctionDefinition(const MemberFunctionDefinition &obj, const std::string &structName)
      : Definition{obj.GetSpan(), DefinitionType::Function}, m_IsMethod{true}
  {
    const MemberFunctionPrototypeDefinition &prototype = *obj.m_Prototype;
    m_AccessSpecifier = prototype.m_AccessSpecifier;
    m_IsAnalyzed = obj.m_IsAnalyzed;

    m_FunctionName = MakeBox<Identifier>(MethodToFunctionName(obj, structName), prototype.m_Name->GetSpan());

    if (prototype.m_Method)
    {
      m_Params.push_back(prototype.m_Method->Clone());
    }

    for (auto &param : prototype.m_Params)
    {
      m_Params.push_back(param->Clone());
    }

    m_Generics = prototype.m_Generics ? MakeBox<Generics>(*prototype.m_Generics.get()) : nullptr;
    m_ReturnType = prototype.m_ReturnType->Clone();
    m_Block = obj.m_Block->Clone();
  }

  FunctionDefinition::FunctionDefinition(MemberFunctionDefinition &&obj, const std::string &structName)
      : Definition{obj.GetSpan(), DefinitionType::Function}, m_IsMethod{true}
  {
    MemberFunctionPrototypeDefinition &prototype = *obj.m_Prototype;
    m_AccessSpecifier = prototype.m_AccessSpecifier;
    m_IsAnalyzed = obj.m_IsAnalyzed;
    m_FunctionName = MakeBox<Identifier>(MethodToFunctionName(obj, structName), prototype.m_Name->GetSpan());

    m_Params.reserve(prototype.m_Params.size());

    if (prototype.m_Method)
    {
      m_Params.push_back(std::move(prototype.m_Method));
    }

    for (auto &param : prototype.m_Params)
    {
      m_Params.push_back(std::move(param));
    }

    m_Generics = std::move(prototype.m_Generics);
    m_ReturnType = prototype.m_ReturnType;
    m_Block = std::move(obj.m_Block);
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

    SymbolTable::Get().Insert(*fnDefinition);
    return fnDefinition;
  }

  std::string FunctionDefinition::MethodToFunctionName(const MemberFunctionDefinition &obj, const std::string &structName)
  {
    return structName + "_" + obj.m_Prototype->m_Name->Id() + "_" + IDGenerator::Generate();
  }

} // namespace Marble
