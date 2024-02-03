use crate::{error::CompilerError, lexer::token_type::TokenType, parser::Parser};

use super::{identifier::Identifier, type_specifier::TypeSpecifier, AstParse};

#[derive(Debug)]
pub struct VariableType {
    pub identifier: Identifier,
    pub type_specifier: TypeSpecifier,
}

impl VariableType {
    pub fn new(identifier: Identifier, type_specifier: TypeSpecifier) -> Self {
        Self {
            identifier,
            type_specifier,
        }
    }
}

impl AstParse for VariableType {
    fn parse(parser: &mut Parser) -> Result<Self, CompilerError> {
        let identifier = Identifier::from(parser.expect(TokenType::Identifier)?);
        parser.next_token_and_expect(TokenType::Colon)?;
        parser.next_token()?;
        return Ok(VariableType::new(identifier, TypeSpecifier::parse(parser)?));
    }
}
