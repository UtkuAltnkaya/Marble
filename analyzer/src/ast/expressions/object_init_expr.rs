use crate::{
    ast::{
        expressions::identifier_expression::IdentifierExpression, identifier::Identifier,
        parenthesis::Parenthesis, AstNode,
    },
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

use super::{Expression, Precedence};

#[derive(Debug)]
pub struct ObjectInitExpression {
    pub object: Box<Expression>,
    pub fields: Vec<Field>,
}

#[derive(Debug)]
pub struct Field {
    pub name: Identifier,
    pub value: Box<Expression>,
}

impl ObjectInitExpression {
    pub fn new(object: Box<Expression>, fields: Vec<Field>) -> Self {
        Self { object, fields }
    }
    pub fn parse(parser: &mut Parser, precedence: Precedence) -> Result<Expression> {
        let left = Expression::parse_expr(parser, precedence.next())?;
        if parser.next().token_type() != &TokenType::OpenCurlyBrace {
            return Ok(left);
        }
        parser.next_token()?;
        let fields = Parenthesis::parse(parser, TokenType::CloseCurlyBrace, |parser| {
            return Field::parse(parser);
        })?;
        return Ok(Expression::ObjectInit(Self::new(Box::from(left), fields)));
    }
}

impl Field {
    pub fn new(name: Identifier, value: Box<Expression>) -> Self {
        Self { name, value }
    }

    pub fn parse(parser: &mut Parser) -> Result<Self> {
        let name = Identifier::from(parser.expect(TokenType::Identifier)?);
        let token_type = parser.next().token_type();
        if token_type == &TokenType::Comma || token_type == &TokenType::CloseCurlyBrace {
            let id_expr = IdentifierExpression::new(name.clone());
            return Ok(Self::new(name, Box::from(Expression::Identifier(id_expr))));
        }

        parser.next_token_and_expect(TokenType::Colon)?;
        parser.next_token()?;
        let value = Expression::parse(parser)?;
        return Ok(Field::new(name, Box::from(value)));
    }
}
