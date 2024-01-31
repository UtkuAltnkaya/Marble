use crate::{
    ast::{
        access_specifier::AccessSpecifier, identifier::Identifier, parenthesis::Parenthesis,
        AstNode,
    },
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

#[derive(Debug)]
pub struct EnumDeclaration {
    pub access_specifier: AccessSpecifier,
    pub name: Identifier,
    pub fields: Vec<Identifier>,
}

impl AstNode for EnumDeclaration {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let name = Identifier::parse(parser)?;
        parser.next_token_and_expect(TokenType::OpenCurlyBrace)?;
        let fields = Parenthesis::parse(parser, TokenType::CloseCurlyBrace, |parser| {
            return Ok(Identifier::from(parser.expect(TokenType::Identifier)?));
        })?;
        return Ok(Self::new(AccessSpecifier::Private, name, fields));
    }
}

impl EnumDeclaration {
    pub fn new(
        access_specifier: AccessSpecifier,
        name: Identifier,
        fields: Vec<Identifier>,
    ) -> Self {
        Self {
            access_specifier,
            name,
            fields,
        }
    }
}
