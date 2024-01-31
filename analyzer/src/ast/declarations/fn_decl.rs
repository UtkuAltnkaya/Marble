use crate::{
    ast::{
        access_specifier::AccessSpecifier, identifier::Identifier, parenthesis::Parenthesis,
        statements::block_stmt::BlockStmt, type_specifier::TypeSpecifier,
        variable_type::VariableType, AstNode,
    },
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

#[derive(Debug)]
pub struct FnDeclaration {
    pub access_specifier: AccessSpecifier,
    pub fn_name: Identifier,
    pub params: Vec<VariableType>,
    pub return_type: TypeSpecifier,
    pub body: BlockStmt,
}

impl AstNode for FnDeclaration {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let fn_name = Identifier::parse(parser)?;
        let params = Self::parse_params(parser)?;
        let return_type = Self::parse_return_type(parser)?;
        let body = Self::parse_body(parser)?;
        return Ok(FnDeclaration::new(
            AccessSpecifier::Private,
            fn_name,
            params,
            return_type,
            body,
        ));
    }
}

impl FnDeclaration {
    pub fn new(
        access_specifier: AccessSpecifier,
        fn_name: Identifier,
        params: Vec<VariableType>,
        return_type: TypeSpecifier,
        body: BlockStmt,
    ) -> Self {
        Self {
            access_specifier,
            fn_name,
            params,
            return_type,
            body,
        }
    }

    fn parse_params(parser: &mut Parser) -> Result<Vec<VariableType>> {
        parser.next_token_and_expect(TokenType::OpenParen)?;
        return Parenthesis::parse(parser, TokenType::CloseParen, |parser| {
            return Ok(VariableType::parse(parser)?);
        });
    }

    fn parse_return_type(parser: &mut Parser) -> Result<TypeSpecifier> {
        parser.next_token_and_expect(TokenType::Arrow)?;
        parser.next_token()?;
        return TypeSpecifier::parse(parser);
    }

    fn parse_body(parser: &mut Parser) -> Result<BlockStmt> {
        parser.next_token()?;
        return BlockStmt::parse(parser);
    }
}
