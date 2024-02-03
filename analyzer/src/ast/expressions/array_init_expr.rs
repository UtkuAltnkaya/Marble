use crate::{
    ast::{parenthesis::Parenthesis, AstParse},
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

use super::{Expression, Precedence};

#[derive(Debug)]
pub struct ArrayInitExpression {
    pub array: Vec<Expression>,
    pub size: usize,
}

impl ArrayInitExpression {
    pub fn new(array: Vec<Expression>, size: usize) -> Self {
        Self { array, size }
    }

    pub fn parse(parser: &mut Parser, precedence: Precedence) -> Result<Expression> {
        if parser.current().token_type() != &TokenType::OpenBracket {
            return Expression::parse_expr(parser, precedence.next());
        }

        let array = Parenthesis::parse(parser, TokenType::CloseBracket, |parser| {
            return Expression::parse(parser);
        })?;
        let size = array.len();

        return Ok(Expression::ArrayInit(Self::new(array, size)));
    }
}
