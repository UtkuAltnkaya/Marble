use crate::{
    ast::{expressions::Expression, AstParse},
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

use super::block_stmt::BlockStmt;

#[derive(Debug)]
pub struct WhileStmt {
    pub condition: Expression,
    pub block: BlockStmt,
}

impl AstParse for WhileStmt {
    fn parse(parser: &mut Parser) -> Result<Self> {
        parser.next_token_and_expect(TokenType::OpenParen)?;
        parser.next_token()?;
        let condition = Expression::parse(parser)?;
        parser.next_token_and_expect(TokenType::CloseParen)?;
        let block = BlockStmt::parse(parser)?;
        return Ok(Self::new(condition, block));
    }
}

impl WhileStmt {
    pub fn new(condition: Expression, block: BlockStmt) -> Self {
        Self { condition, block }
    }
}
