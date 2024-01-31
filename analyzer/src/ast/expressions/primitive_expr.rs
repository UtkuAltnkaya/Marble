use crate::{
    ast::{type_specifier::TypeSpecifier, AstNode},
    error::Result,
    error_parser,
    lexer::token_type::TokenType,
    parser::Parser,
};

use super::Expression;

#[derive(Debug)]
pub struct PrimitiveExpression {
    pub type_specifier: TypeSpecifier,
    pub value: String,
}

impl PrimitiveExpression {
    pub fn new(type_specifier: TypeSpecifier, value: String) -> Self {
        Self {
            type_specifier,
            value,
        }
    }

    pub fn parse(parser: &mut Parser) -> Result<Expression> {
        if let Some(expr) = Self::parse_parenthesis(parser)? {
            return Ok(expr);
        }
        let token = parser.current();
        return Ok(Expression::Primitive(match token.token_type() {
            TokenType::Number => {
                let type_specifier = if parser.current().text().contains(".") {
                    TypeSpecifier::Double
                } else {
                    TypeSpecifier::Int
                };
                Self::new(type_specifier, token.text().to_string())
            }
            TokenType::String => Self::new(TypeSpecifier::Str, token.text().to_string()),
            TokenType::Char => Self::new(TypeSpecifier::Char, token.text().to_string()),
            TokenType::True => Self::new(TypeSpecifier::Bool, token.text().to_string()),
            TokenType::False => Self::new(TypeSpecifier::Bool, token.text().to_string()),
            _ => return error_parser!(parser, "Unknown Expression"),
        }));
    }

    fn parse_parenthesis(parser: &mut Parser) -> Result<Option<Expression>> {
        if parser.current().token_type() != &TokenType::OpenParen {
            return Ok(None);
        }
        parser.next_token()?;
        let expr = Expression::parse(parser)?;
        parser.next_token_and_expect(TokenType::CloseParen)?;
        return Ok(Some(expr));
    }
}
