use crate::{
    ast::{expressions::Expression, AstParse},
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

impl AstParse for BlockStmt {
    fn parse(parser: &mut Parser) -> Result<Self> {
        parser.next_token_and_expect(TokenType::OpenCurlyBrace)?;
        let stmts = Self::parse_loop(parser)?;
        return Ok(BlockStmt::new(stmts));
    }
}

impl BlockStmt {
    pub fn new(stmts: Vec<Statements>) -> Self {
        Self { stmts }
    }

    fn parse_loop(parser: &mut Parser) -> Result<Vec<Statements>> {
        let mut vec = Vec::new();
        loop {
            parser.next_token()?;
            if parser.current().token_type() == &TokenType::CloseCurlyBrace {
                break;
            }
            if parser.current().token_type() == &TokenType::Eof {
                return error_parser!(parser, "Missing close curly brace '}'");
            }

            if Self::is_statement(parser) {
                vec.push(Statements::parse(parser)?);
            } else if parser.current().token_type() == &TokenType::OpenCurlyBrace {
                vec.push(Statements::Block(Self::new(Self::parse_loop(parser)?)));
            } else {
                let expr = Expression::parse(parser)?;
                parser.next_token_and_expect(TokenType::Semicolon)?;
                vec.push(Statements::Expression(expr));
            }
        }
        return Ok(vec);
    }

    fn is_statement(parser: &Parser) -> bool {
        return matches!(
            parser.current().token_type(),
            TokenType::Let
                | TokenType::Return
                | TokenType::If
                | TokenType::For
                | TokenType::While
                | TokenType::Defer
        );
    }
}
