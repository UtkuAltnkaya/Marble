pub mod block_stmt;
pub mod defer_stmt;
pub mod for_stmt;
pub mod if_stmt;
pub mod let_stmt;
pub mod return_stmt;
pub mod while_stmt;

use crate::{error::CompilerError, error_parser, lexer::token_type::TokenType, parser::Parser};

use self::{
    block_stmt::BlockStmt, defer_stmt::DeferStmt, for_stmt::ForStmt, if_stmt::IfStmt,
    let_stmt::LetStmt, return_stmt::ReturnStmt, while_stmt::WhileStmt,
};

use super::{expressions::Expression, AstParse};

#[derive(Debug)]
pub enum Statements {
    Let(LetStmt),
    Return(ReturnStmt),
    Defer(DeferStmt),
    For(ForStmt),
    While(WhileStmt),
    If(IfStmt),
    Block(BlockStmt),
    Expression(Expression),
}

impl AstParse for Statements {
    fn parse(parser: &mut Parser) -> Result<Self, CompilerError> {
        return Ok(match parser.current().token_type() {
            TokenType::Let => Statements::Let(LetStmt::parse(parser)?),
            TokenType::Return => Statements::Return(ReturnStmt::parse(parser)?),
            TokenType::OpenCurlyBrace => Statements::Block(BlockStmt::parse(parser)?),
            TokenType::Defer => Statements::Defer(DeferStmt::parse(parser)?),
            TokenType::For => Statements::For(ForStmt::parse(parser)?),
            TokenType::While => Statements::While(WhileStmt::parse(parser)?),
            TokenType::If => Statements::If(IfStmt::parse(parser)?),
            _ => return error_parser!(parser, "Unknown Statement"),
        });
    }
}
