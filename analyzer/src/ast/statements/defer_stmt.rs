use crate::{
    ast::{expressions::Expression, AstParse},
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

#[derive(Debug)]
pub struct DeferStmt {
    pub expr: Expression,
}

impl AstParse for DeferStmt {
    fn parse(parser: &mut Parser) -> Result<Self> {
        parser.next_token()?; // Skip defer keyword
        let expr = Expression::parse(parser)?;
        parser.next_token_and_expect(TokenType::Semicolon)?;
        return Ok(DeferStmt::new(expr));
    }
}

impl DeferStmt {
    pub fn new(expr: Expression) -> Self {
        Self { expr }
    }
}
