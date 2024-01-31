use crate::{
    ast::identifier::Identifier, error::Result, lexer::token_type::TokenType, parser::Parser,
};

use super::{Expression, Precedence};

#[derive(Debug)]
pub struct IdentifierExpression {
    pub id: Identifier,
}

impl IdentifierExpression {
    pub fn new(id: Identifier) -> Self {
        Self { id }
    }

    pub fn parse(parser: &mut Parser, precedence: Precedence) -> Result<Expression> {
        let token = parser.current();
        if token.token_type() != &TokenType::Identifier {
            return Expression::parse_expr(parser, precedence.next());
        }
        let id = Identifier::from(token.text());
        return Ok(Expression::Identifier(IdentifierExpression::new(id)));
    }
}
