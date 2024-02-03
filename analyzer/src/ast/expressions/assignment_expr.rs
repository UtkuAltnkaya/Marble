use crate::{error::Result, lexer::token_type::TokenType, parser::Parser};

use super::{Expression, Precedence};

#[derive(Debug)]
pub struct AssignmentExpression {
    pub variable: Box<Expression>,
    pub value: Box<Expression>,
}

impl AssignmentExpression {
    pub fn new(variable: Box<Expression>, value: Box<Expression>) -> Self {
        Self { variable, value }
    }

    pub fn parse(parser: &mut Parser, precedence: Precedence) -> Result<Expression> {
        let left = Expression::parse_expr(parser, precedence.next())?;

        if parser.next().token_type() != &TokenType::Assign {
            return Ok(left);
        }

        parser.next_token()?;
        parser.next_token()?;
        let right = Expression::parse_expr(parser, Precedence::default())?;

        return Ok(Expression::Assignment(AssignmentExpression::new(
            Box::from(left),
            Box::from(right),
        )));
    }
}
