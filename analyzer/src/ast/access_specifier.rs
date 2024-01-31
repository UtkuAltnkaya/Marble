use crate::{error::CompilerError, lexer::token_type::TokenType, parser::Parser};

use super::AstNode;

#[derive(Debug)]
pub enum AccessSpecifier {
    Public,
    Private,
}

impl AstNode for AccessSpecifier {
    fn parse(parser: &mut Parser) -> Result<Self, CompilerError> {
        if parser.current().token_type() == &TokenType::Pub {
            parser.next_token()?;
            return Ok(Self::Public);
        }
        return Ok(Self::Private);
    }
}
