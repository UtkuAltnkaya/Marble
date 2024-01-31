use crate::{ast::AstNode, error::Result, lexer::token_type::TokenType, parser::Parser};

use super::{Expression, Precedence};

#[derive(Debug)]
pub struct ArrayIndexExpression {
    pub array: Box<Expression>,
    pub index: Box<Expression>,
}

impl ArrayIndexExpression {
    pub fn new(array: Box<Expression>, index: Box<Expression>) -> Self {
        Self { array, index }
    }

    pub fn parse(parser: &mut Parser, precedence: Precedence) -> Result<Expression> {
        let left = Expression::parse_expr(parser, precedence.next())?;
        if parser.next().token_type() != &TokenType::OpenBracket {
            return Ok(left);
        }
        parser.next_token()?;
        parser.next_token()?;
        let right = Expression::parse(parser)?;
        parser.next_token_and_expect(TokenType::CloseBracket)?;
        return Ok(Expression::ArrayIndex(Self::new(
            Box::from(left),
            Box::from(right),
        )));
    }
}
