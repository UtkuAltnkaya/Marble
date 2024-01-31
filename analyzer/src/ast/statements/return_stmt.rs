use crate::{
    ast::{expressions::Expression, AstNode},
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

#[derive(Debug)]
pub struct ReturnStmt {
    pub value: Option<Expression>,
}

impl ReturnStmt {
    pub fn new(value: Option<Expression>) -> Self {
        Self { value }
    }
}

impl AstNode for ReturnStmt {
    fn parse(parser: &mut Parser) -> Result<Self> {
        if parser.next().token_type() == &TokenType::Semicolon {
            parser.next_token()?;
            return Ok(ReturnStmt::new(None));
        }
        parser.next_token()?;
        let value = Expression::parse(parser)?;
        parser.next_token_and_expect(TokenType::Semicolon)?;
        return Ok(ReturnStmt::new(Some(value)));
    }
}
