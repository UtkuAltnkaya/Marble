use crate::{
    ast::{type_specifier::TypeSpecifier, AstParse},
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

use super::{Expression, Precedence};

#[derive(Debug)]
pub struct CastExpression {
    pub target_type: TypeSpecifier,
    pub expression: Box<Expression>,
}

impl CastExpression {
    pub fn new(target_type: TypeSpecifier, expression: Box<Expression>) -> Self {
        Self {
            target_type,
            expression,
        }
    }

    pub fn parse(parser: &mut Parser, precedence: Precedence) -> Result<Expression> {
        let left = Expression::parse_expr(parser, precedence.next())?;
        if parser.next().token_type() != &TokenType::As {
            return Ok(left);
        }
        parser.next_token()?;
        parser.next_token()?;

        let target_type = TypeSpecifier::parse(parser)?;
        let expr = CastExpression::new(target_type, Box::new(left));
        return Ok(Expression::Cast(expr));
    }
}
