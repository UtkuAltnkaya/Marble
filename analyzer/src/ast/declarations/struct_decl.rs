use crate::{
    ast::{
        access_specifier::AccessSpecifier, identifier::Identifier, parenthesis::Parenthesis,
        variable_type::VariableType, AstNode,
    },
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

#[derive(Debug)]
pub struct StructDeclaration {
    pub access_specifier: AccessSpecifier,
    pub name: Identifier,
    pub fields: Vec<StructField>,
}

#[derive(Debug)]
pub struct StructField {
    pub access_specifier: AccessSpecifier,
    pub field: VariableType,
}

impl AstNode for StructDeclaration {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let name = Identifier::parse(parser)?;
        parser.next_token_and_expect(TokenType::OpenCurlyBrace)?;
        let fields = Parenthesis::parse(parser, TokenType::CloseCurlyBrace, |parser| {
            return StructField::parse(parser);
        })?;
        return Ok(Self::new(AccessSpecifier::Private, name, fields));
    }
}

impl StructDeclaration {
    pub fn new(
        access_specifier: AccessSpecifier,
        name: Identifier,
        fields: Vec<StructField>,
    ) -> Self {
        Self {
            access_specifier,
            name,
            fields,
        }
    }
}
impl AstNode for StructField {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let access_specifier = AccessSpecifier::parse(parser)?;
        let field = VariableType::parse(parser)?;
        return Ok(Self::new(access_specifier, field));
    }
}

impl StructField {
    pub fn new(access_specifier: AccessSpecifier, field: VariableType) -> Self {
        Self {
            access_specifier,
            field,
        }
    }
}