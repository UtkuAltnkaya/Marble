use crate::{
    ast::{expressions::Expression, AstNode},
    error::Result,
    error_parser,
    lexer::token_type::TokenType,
    parser::Parser,
};

use super::Statements;

#[derive(Debug)]
pub struct BlockStmt {
    pub stmts: Vec<Statements>,
}

impl AstNode for BlockStmt {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let mut vec = Vec::new();
        loop {
            parser.next_token()?;
            if parser.current().token_type() == &TokenType::CloseCurlyBrace {
                break;
            }
            if parser.current().token_type() == &TokenType::Eof {
                return error_parser!(parser, "Missing close curly brace '}'");
            }
            if !Self::is_statement(parser) {
                let expr = Expression::parse(parser)?;
                parser.next_token_and_expect(TokenType::Semicolon)?;
                vec.push(Statements::Expression(expr));
            } else {
                vec.push(Statements::parse(parser)?);
            }
        }

        return Ok(BlockStmt::new(vec));
    }
}

impl BlockStmt {
    pub fn new(stmts: Vec<Statements>) -> Self {
        Self { stmts }
    }
    fn is_statement(parser: &Parser) -> bool {
        return matches!(
            parser.current().token_type(),
            TokenType::Let
                | TokenType::Return
                | TokenType::If
                | TokenType::For
                | TokenType::While
                | TokenType::OpenCurlyBrace
                | TokenType::Defer
        );
    }
}
