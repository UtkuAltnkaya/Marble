use crate::{
    ast::{parenthesis::Parenthesis, AstNode},
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

use super::{Expression, Precedence};

#[derive(Debug)]
pub struct FnCallExpression {
    pub fn_name: Box<Expression>,
    pub namespace: Option<NameSpace>,
    pub args: Vec<Expression>,
}

#[derive(Debug)]
pub struct NameSpace {
    pub name: Box<Expression>,
}

impl FnCallExpression {
    pub fn new(
        fn_name: Box<Expression>,
        namespace: Option<NameSpace>,
        args: Vec<Expression>,
    ) -> Self {
        Self {
            fn_name,
            namespace,
            args,
        }
    }

    pub fn parse(parser: &mut Parser, precedence: Precedence) -> Result<Expression> {
        let left = Expression::parse_expr(parser, precedence.next())?;
        let (namespace, left) = NameSpace::parse(parser, precedence, left)?;

        if parser.next().token_type() != &TokenType::OpenParen {
            return Ok(left);
        }
        parser.next_token()?;
        let args = Parenthesis::parse(parser, TokenType::CloseParen, |parser| {
            return Expression::parse(parser);
        })?;
        return Ok(Expression::FnCall(Self::new(
            Box::from(left),
            namespace,
            args,
        )));
    }
}

impl NameSpace {
    pub fn new(name: Box<Expression>) -> Self {
        Self { name }
    }

    pub fn parse(
        parser: &mut Parser,
        precedence: Precedence,
        left: Expression,
    ) -> Result<(Option<Self>, Expression)> {
        if parser.next().token_type() != &TokenType::Colon {
            return Ok((None, left));
        }
        parser.next_token()?;
        parser.next_token_and_expect(TokenType::Colon)?;
        parser.next_token()?;
        let namespace = NameSpace::new(Box::from(left));
        return Ok((
            Some(namespace),
            Expression::parse_expr(parser, precedence.next())?,
        ));
    }
}
