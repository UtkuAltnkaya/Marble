use crate::{error::Result, lexer::token_type::TokenType, parser::Parser};

use super::{Expression, Precedence};

#[derive(Debug)]
pub struct MemberAccessExpression {
    pub object: Box<Expression>,
    pub access: TokenType,
    pub property: Box<Expression>,
}

impl MemberAccessExpression {
    pub fn new(object: Box<Expression>, access: TokenType, property: Box<Expression>) -> Self {
        Self {
            object,
            access,
            property,
        }
    }

    pub fn parse(parser: &mut Parser, precedence: Precedence) -> Result<Expression> {
        let left = Expression::parse_expr(parser, precedence.next())?;
        let token_type = parser.next().token_type();

        if token_type != &TokenType::Dot && token_type != &TokenType::Arrow {
            return Ok(left);
        }
        let access = token_type.clone();
        parser.next_token()?;
        parser.next_token()?;
        let property = Expression::parse_expr(parser, precedence)?;
        return Ok(Expression::MemberAccess(Self::new(
            Box::from(left),
            access,
            Box::from(property),
        )));
    }
}
