use super::{Expression, Precedence};
use crate::{
    error::{CompilerError, Result},
    lexer::token_type::TokenType,
    parser::Parser,
};
use std::str::FromStr;

#[derive(Debug)]
pub struct BinaryExpression {
    pub left: Box<Expression>,
    pub operator: BinaryOperators,
    pub right: Box<Expression>,
}

#[derive(Debug)]
pub enum BinaryOperators {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Equal,
    NotEqual,
    And,
    Or,
    BitAnd,
    BitOr,
    BitXor,
    GreaterThan,
    LessThan,
    GreaterThanOrEqual,
    LessThanOrEqual,
    BitLeft,
    BitRight,
}

pub enum BinaryPrecedence {
    Or,
    And,
    BitOr,
    BitXor,
    BitAnd,
    BitLeft,
    BitRight,
    Equal,
    Relational,
    Sum,
    Product,
}

//TODO FIX Is valid token, Maybe ? -> rewrite the parse function for every binary precedence
impl BinaryExpression {
    pub fn new(left: Box<Expression>, operator: BinaryOperators, right: Box<Expression>) -> Self {
        Self {
            left,
            operator,
            right,
        }
    }

    pub(super) fn parse(
        parser: &mut Parser,
        precedence: &Precedence,
        binary_precedence: BinaryPrecedence,
    ) -> Result<Expression> {
        match binary_precedence {
            BinaryPrecedence::Or => Self::parse_or(parser, precedence, binary_precedence),
            BinaryPrecedence::And => {
                Self::parse_expression(parser, precedence, binary_precedence, |token_type| {
                    matches!(token_type, TokenType::And)
                })
            }
            BinaryPrecedence::BitOr => {
                Self::parse_expression(parser, precedence, binary_precedence, |token_type| {
                    matches!(token_type, TokenType::BitOr)
                })
            }
            BinaryPrecedence::BitXor => {
                Self::parse_expression(parser, precedence, binary_precedence, |token_type| {
                    matches!(token_type, TokenType::BitXor)
                })
            }
            BinaryPrecedence::BitAnd => {
                Self::parse_expression(parser, precedence, binary_precedence, |token_type| {
                    matches!(token_type, TokenType::BitAnd)
                })
            }
            BinaryPrecedence::BitLeft => {
                Self::parse_expression(parser, precedence, binary_precedence, |token_type| {
                    matches!(token_type, TokenType::BitLeft)
                })
            }
            BinaryPrecedence::BitRight => {
                Self::parse_expression(parser, precedence, binary_precedence, |token_type| {
                    matches!(token_type, TokenType::BitLeft)
                })
            }
            BinaryPrecedence::Equal => {
                Self::parse_expression(parser, precedence, binary_precedence, |token_type| {
                    matches!(token_type, TokenType::Equal | TokenType::NotEqual)
                })
            }
            BinaryPrecedence::Relational => {
                Self::parse_expression(parser, precedence, binary_precedence, |token_type| {
                    matches!(
                        token_type,
                        TokenType::LessThan
                            | TokenType::LessEqual
                            | TokenType::GreaterThan
                            | TokenType::GreaterEqual
                    )
                })
            }
            BinaryPrecedence::Sum => {
                Self::parse_expression(parser, precedence, binary_precedence, |token_type| {
                    matches!(token_type, TokenType::Plus | TokenType::Minus)
                })
            }
            BinaryPrecedence::Product => Self::parse_product(parser, precedence, binary_precedence),
        }
    }

    fn parse_or(
        parser: &mut Parser,
        precedence: &Precedence,
        binary_precedence: BinaryPrecedence,
    ) -> Result<Expression> {
        let left = Self::parse(parser, precedence, binary_precedence.next())?;
        if parser.next().token_type() != &TokenType::Or {
            return Ok(left);
        }

        parser.next_token()?;
        let op = BinaryOperators::from_str(parser.current().text())?;
        parser.next_token()?;
        let right = Self::parse(parser, precedence, binary_precedence)?;

        return Ok(Expression::Binary(BinaryExpression::new(
            Box::from(left),
            op,
            Box::from(right),
        )));
    }

    fn parse_expression(
        parser: &mut Parser,
        precedence: &Precedence,
        binary_precedence: BinaryPrecedence,
        condition: impl Fn(&TokenType) -> bool,
    ) -> Result<Expression> {
        let left = Self::parse(parser, precedence, binary_precedence.next())?;
        if !condition(parser.next().token_type()) {
            return Ok(left);
        }
        parser.next_token()?;
        let op = BinaryOperators::from_str(parser.current().text())?;
        parser.next_token()?;
        let right = Self::parse(parser, precedence, binary_precedence)?;
        return Ok(Expression::Binary(BinaryExpression::new(
            Box::from(left),
            op,
            Box::from(right),
        )));
    }

    fn parse_product(
        parser: &mut Parser,
        precedence: &Precedence,
        binary_precedence: BinaryPrecedence,
    ) -> Result<Expression> {
        let left = Expression::parse_expr(parser, precedence.next())?;

        if !matches!(
            parser.next().token_type(),
            TokenType::Multiply | TokenType::Divide | TokenType::Percent
        ) {
            return Ok(left);
        }

        parser.next_token()?;
        let op = BinaryOperators::from_str(parser.current().text())?;
        parser.next_token()?;
        let right = Self::parse(parser, precedence, binary_precedence)?;
        return Ok(Expression::Binary(BinaryExpression::new(
            Box::from(left),
            op,
            Box::from(right),
        )));
    }
}

impl Default for BinaryPrecedence {
    fn default() -> Self {
        return Self::Or;
    }
}

impl BinaryPrecedence {
    pub fn next(&self) -> Self {
        match *self {
            Self::Or => Self::And,
            Self::And => Self::BitOr,
            Self::BitOr => Self::BitXor,
            Self::BitXor => Self::BitAnd,
            Self::BitAnd => Self::BitLeft,
            Self::BitLeft => Self::BitRight,
            Self::BitRight => Self::Equal,
            Self::Equal => Self::Relational,
            Self::Relational => Self::Sum,
            Self::Sum => Self::Product,
            Self::Product => Self::Product,
        }
    }
}

impl FromStr for BinaryOperators {
    type Err = CompilerError;

    fn from_str(s: &str) -> Result<Self> {
        return Ok(match s {
            "+" => Self::Add,
            "-" => Self::Subtract,
            "*" => Self::Multiply,
            "/" => Self::Divide,
            "%" => Self::Modulo,
            "==" => Self::Equal,
            "!=" => Self::NotEqual,
            "&&" => Self::And,
            "||" => Self::Or,
            "&" => Self::BitAnd,
            "|" => Self::BitOr,
            "^" => Self::BitXor,
            "<" => Self::LessThan,
            ">" => Self::GreaterThan,
            "<=" => Self::LessThanOrEqual,
            ">=" => Self::GreaterThanOrEqual,
            ">>" => Self::BitRight,
            "<<" => Self::BitLeft,
            _ => unreachable!(),
        });
    }
}
