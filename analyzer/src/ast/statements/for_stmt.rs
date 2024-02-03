use crate::{
    ast::{
        expressions::{assignment_expr::AssignmentExpression, Expression},
        AstParse,
    },
    error::Result,
    error_parser,
    lexer::token_type::TokenType,
    parser::Parser,
};

use super::{block_stmt::BlockStmt, let_stmt::LetStmt};

#[derive(Debug)]
pub struct ForStmt {
    pub init: VariableInit,
    pub condition: Expression,
    pub increment: Expression,
    pub block: BlockStmt,
}
#[derive(Debug)]
pub enum VariableInit {
    Let(LetStmt),
    Assign(AssignmentExpression),
}

impl AstParse for ForStmt {
    fn parse(parser: &mut Parser) -> Result<Self> {
        parser.next_token_and_expect(TokenType::OpenParen)?;
        let init = VariableInit::parse(parser)?;
        parser.next_token()?;
        let condition = Expression::parse(parser)?;
        parser.next_token_and_expect(TokenType::Semicolon)?;
        parser.next_token()?;
        let increment = Expression::parse(parser)?;
        parser.next_token_and_expect(TokenType::CloseParen)?;
        let block = BlockStmt::parse(parser)?;
        return Ok(Self::new(init, condition, increment, block));
    }
}

impl ForStmt {
    pub fn new(
        init: VariableInit,
        condition: Expression,
        increment: Expression,
        block: BlockStmt,
    ) -> Self {
        Self {
            init,
            condition,
            increment,
            block,
        }
    }
}

impl AstParse for VariableInit {
    fn parse(parser: &mut Parser) -> Result<Self> {
        parser.next_token()?;
        if parser.current().token_type() == &TokenType::Let {
            return Ok(VariableInit::Let(LetStmt::parse(parser)?));
        }
        let expr = Expression::parse(parser)?;

        if let Expression::Assignment(expr) = expr {
            parser.next_token()?;
            return Ok(VariableInit::Assign(expr));
        }
        return error_parser!(parser, "Expect assignment expression");
    }
}
