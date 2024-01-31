use crate::{error::Result, lexer::token_type::TokenType, parser::Parser};

use super::{Expression, Precedence};

#[derive(Debug)]
pub struct UnaryExpression {
    pub operator: UnaryOperators,
    pub value: Box<Expression>,
    pub expression_type: UnaryExpressionType,
}

#[derive(Debug)]
pub enum UnaryOperators {
    Plus,
    Minus,
    Increment,
    Decrement,
    LogicalNot,
    BitwiseNot,
    Address,
    Indirection,
}

#[derive(Debug)]
pub enum UnaryExpressionType {
    Prefix,
    Postfix,
}

impl UnaryExpression {
    pub fn new(
        operator: UnaryOperators,
        value: Box<Expression>,
        expression_type: UnaryExpressionType,
    ) -> Self {
        Self {
            operator,
            value,
            expression_type,
        }
    }

    pub fn parse(parser: &mut Parser, precedence: Precedence) -> Result<Expression> {
        if let Some(prefix) = Self::parse_prefix(parser, &precedence)? {
            return Ok(prefix);
        }

        let left = Expression::parse_expr(parser, precedence.next())?;

        let operator = match parser.next().token_type() {
            TokenType::Increment => UnaryOperators::Increment,
            TokenType::Decrement => UnaryOperators::Decrement,
            _ => return Ok(left),
        };
        parser.next_token()?;
        return Ok(Expression::Unary(UnaryExpression::new(
            operator,
            Box::from(left),
            UnaryExpressionType::Postfix,
        )));
    }

    pub fn parse_prefix(
        parser: &mut Parser,
        precedence: &Precedence,
    ) -> Result<Option<Expression>> {
        let operator = if let Ok(operator) = UnaryOperators::try_from(parser.current().token_type())
        {
            operator
        } else {
            return Ok(None);
        };
        parser.next_token()?;
        let value = Expression::parse_expr(parser, precedence.next())?;
        let expr = UnaryExpression::new(operator, Box::from(value), UnaryExpressionType::Prefix);
        return Ok(Some(Expression::Unary(expr)));
    }
}

impl TryFrom<&TokenType> for UnaryOperators {
    type Error = bool;

    fn try_from(value: &TokenType) -> std::result::Result<Self, Self::Error> {
        return Ok(match value {
            TokenType::Increment => UnaryOperators::Increment,
            TokenType::Decrement => UnaryOperators::Decrement,
            TokenType::Plus => UnaryOperators::Plus,
            TokenType::Minus => UnaryOperators::Minus,
            TokenType::Multiply => UnaryOperators::Indirection,
            TokenType::BitAnd => UnaryOperators::Address,
            TokenType::Tilde => UnaryOperators::BitwiseNot,
            TokenType::Bang => UnaryOperators::LogicalNot,
            _ => return Err(false),
        });
    }
}
