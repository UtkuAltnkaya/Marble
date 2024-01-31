use super::{Expression, Precedence};
use crate::{
    ast::AstNode,
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
    BitOr,
    BitXor,
    BitAnd,
    BitLeft,
    BitRight,
    Or,
    And,
    Equal,
    Relational,
    Sum,
    Product,
}

impl AstNode for BinaryExpression {}

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
            BinaryPrecedence::BitOr => Self::parse_bitor(parser, precedence, binary_precedence),
            BinaryPrecedence::BitXor => {
                Self::parse_expression(parser, precedence, binary_precedence)
            }
            BinaryPrecedence::BitAnd => {
                Self::parse_expression(parser, precedence, binary_precedence)
            }
            BinaryPrecedence::BitLeft => {
                Self::parse_expression(parser, precedence, binary_precedence)
            }
            BinaryPrecedence::BitRight => {
                Self::parse_expression(parser, precedence, binary_precedence)
            }
            BinaryPrecedence::Or => Self::parse_expression(parser, precedence, binary_precedence),
            BinaryPrecedence::And => Self::parse_expression(parser, precedence, binary_precedence),
            BinaryPrecedence::Equal => {
                Self::parse_expression(parser, precedence, binary_precedence)
            }
            BinaryPrecedence::Relational => {
                Self::parse_expression(parser, precedence, binary_precedence)
            }
            BinaryPrecedence::Sum => Self::parse_expression(parser, precedence, binary_precedence),
            BinaryPrecedence::Product => Self::parse_product(parser, precedence, binary_precedence),
        }
    }

    fn parse_bitor(
        parser: &mut Parser,
        precedence: &Precedence,
        binary_precedence: BinaryPrecedence,
    ) -> Result<Expression> {
        let left = Self::parse(parser, precedence, binary_precedence.next())?;
        if parser.next().token_type() != &TokenType::BitOr {
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
    ) -> Result<Expression> {
        let left = Self::parse(parser, precedence, binary_precedence.next())?;
        if !Self::is_valid_token(parser) {
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

    fn is_valid_token(parser: &Parser) -> bool {
        return matches!(
            parser.next().token_type(),
            TokenType::Plus
                | TokenType::Minus
                | TokenType::Equal
                | TokenType::NotEqual
                | TokenType::LessThan
                | TokenType::GreaterThan
                | TokenType::LessEqual
                | TokenType::BitAnd
                | TokenType::BitOr
                | TokenType::BitXor
                | TokenType::BitLeft
                | TokenType::BitRight
                | TokenType::And
                | TokenType::GreaterEqual
        );
    }
}

impl Default for BinaryPrecedence {
    fn default() -> Self {
        return Self::BitOr;
    }
}

impl BinaryPrecedence {
    pub fn next(&self) -> Self {
        match *self {
            Self::BitOr => Self::BitXor,
            Self::BitXor => Self::BitAnd,
            Self::BitAnd => Self::BitLeft,
            Self::BitLeft => Self::BitRight,
            Self::BitRight => Self::Or,
            Self::Or => Self::And,
            Self::And => Self::Equal,
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
            _ => unreachable!(),
        });
    }
}
