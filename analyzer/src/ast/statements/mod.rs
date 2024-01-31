pub mod block_stmt;
pub mod defer_stmt;
pub mod let_stmt;
pub mod return_stmt;

use crate::{error::CompilerError, error_parser, lexer::token_type::TokenType, parser::Parser};

use self::{
    block_stmt::BlockStmt, defer_stmt::DeferStmt, let_stmt::LetStmt, return_stmt::ReturnStmt,
};

use super::{expressions::Expression, AstNode};

#[derive(Debug)]
pub enum Statements {
    Let(LetStmt),
    Return(ReturnStmt),
    Defer(DeferStmt),
    For(),
    While(),
    Block(BlockStmt),
    Expression(Expression),
}

impl AstNode for Statements {
    fn parse(parser: &mut Parser) -> Result<Self, CompilerError> {
        return Ok(match parser.current().token_type() {
            TokenType::Let => Statements::Let(LetStmt::parse(parser)?),
            TokenType::Return => Statements::Return(ReturnStmt::parse(parser)?),
            TokenType::OpenCurlyBrace => Statements::Block(BlockStmt::parse(parser)?),
            TokenType::Defer => Statements::Defer(DeferStmt::parse(parser)?),
            TokenType::For => todo!(),
            TokenType::While => todo!(),
            _ => return error_parser!(parser, "Unknown Statement"),
        });
    }
}
