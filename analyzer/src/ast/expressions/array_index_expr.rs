use crate::{ast::AstParse, error::Result, lexer::token_type::TokenType, parser::Parser};

use super::{Expression, Precedence};

#[derive(Debug)]
pub struct ArrayIndexExpression {
    pub array: Box<Expression>,
    pub index: Box<Expression>,
    pub second_index: Option<Box<Expression>>,
}

impl ArrayIndexExpression {
    pub fn new(
        array: Box<Expression>,
        index: Box<Expression>,
        second_index: Option<Box<Expression>>,
    ) -> Self {
        Self {
            array,
            index,
            second_index,
        }
    }

    pub fn parse(parser: &mut Parser, precedence: Precedence) -> Result<Expression> {
        let left = Expression::parse_expr(parser, precedence.next())?;
        if parser.next().token_type() != &TokenType::OpenBracket {
            return Ok(left);
        }
        parser.next_token()?; // Skip array_name
        parser.next_token()?; // Skip open bracket token
        let right = Expression::parse(parser)?;

        let mut second_index: Option<Box<Expression>> = None;
        if parser.next().token_type() == &TokenType::Comma {
            parser.next_token()?; // skip expression
            parser.next_token()?; // skip comma
            second_index = Some(Box::new(Expression::parse(parser)?));
        }

        parser.next_token_and_expect(TokenType::CloseBracket)?;
        return Ok(Expression::ArrayIndex(Self::new(
            Box::from(left),
            Box::from(right),
            second_index,
        )));
    }
}
