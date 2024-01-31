use crate::{
    ast::AstNode,
    error::Result,
    lexer::{token::Token, token_type::TokenType},
    parser::Parser,
};

#[derive(Debug, Clone)]
pub struct Identifier {
    pub id: String,
}

impl Identifier {
    pub fn new(id: String) -> Self {
        Self { id }
    }
}

impl AstNode for Identifier {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let token = parser.next_token_and_expect(TokenType::Identifier)?;
        return Ok(Identifier::new(token.text().to_owned()));
    }
}

impl From<&str> for Identifier {
    fn from(value: &str) -> Self {
        return Self::new(value.to_owned());
    }
}

impl From<&Token> for Identifier {
    fn from(value: &Token) -> Self {
        return Self::new(value.text().to_owned());
    }
}