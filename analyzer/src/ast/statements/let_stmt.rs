use crate::{
    ast::{
        expressions::Expression, identifier::Identifier, type_specifier::TypeSpecifier, AstNode,
    },
    debug_current_token,
    error::Result,
    error_parser,
    lexer::token_type::TokenType,
    parser::Parser,
};

#[derive(Debug)]
pub struct LetStmt {
    pub identifier: Identifier,
    pub type_specifier: Option<TypeSpecifier>,
    pub value: Option<Expression>,
}

impl AstNode for LetStmt {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let identifier = Identifier::parse(parser)?;
        parser.next_token()?;
        let type_specifier = Self::handle_type(parser)?;

        if parser.current().token_type() == &TokenType::Semicolon {
            if type_specifier.is_some() {
                return Ok(LetStmt::new(identifier, type_specifier, None));
            }
            return error_parser!(parser, "Expect TypeSpecifier");
        }

        parser.expect(TokenType::Assign)?;
        parser.next_token()?;
        debug_current_token!(parser);
        let value = Expression::parse(parser)?;
        parser.next_token_and_expect(TokenType::Semicolon)?;
        return Ok(LetStmt::new(identifier, type_specifier, Some(value)));
    }
}

impl LetStmt {
    pub fn new(
        identifier: Identifier,
        type_specifier: Option<TypeSpecifier>,
        value: Option<Expression>,
    ) -> Self {
        Self {
            identifier,
            type_specifier,
            value,
        }
    }

    fn handle_type(parser: &mut Parser) -> Result<Option<TypeSpecifier>> {
        if parser.current().token_type() != &TokenType::Colon {
            return Ok(None);
        }
        parser.next_token()?;
        let type_specifier = TypeSpecifier::parse(parser)?;
        parser.next_token()?;
        return Ok(Some(type_specifier));
    }
}
