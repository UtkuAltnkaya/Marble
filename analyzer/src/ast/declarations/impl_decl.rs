use crate::{
    ast::{
        access_specifier::AccessSpecifier, declarations::fn_decl::FnDeclaration,
        identifier::Identifier, statements::block_stmt::BlockStmt, type_specifier::TypeSpecifier,
        variable_type::VariableType, AstParse,
    },
    error::Result,
    error_parser,
    lexer::token_type::TokenType,
    parser::Parser,
};

#[derive(Debug)]
pub struct ImplDeclaration {
    pub name: TypeSpecifier,
    pub member_functions: Vec<MemberFunction>,
}

#[derive(Debug)]
pub struct MemberFunction {
    pub prototype: MemberFunctionPrototype,
    pub body: BlockStmt,
}

#[derive(Debug)]
pub struct MemberFunctionPrototype {
    pub access_specifier: AccessSpecifier,
    pub method: Option<VariableType>,
    pub name: Identifier,
    pub params: Vec<VariableType>,
    pub return_type: TypeSpecifier,
}

impl AstParse for ImplDeclaration {
    fn parse(parser: &mut Parser) -> Result<Self> {
        parser.next_token()?; // skip impl token
        let name = TypeSpecifier::parse(parser)?;

        let mut member_func = Vec::new();
        parser.next_token_and_expect(TokenType::OpenCurlyBrace)?;
        loop {
            parser.next_token()?;
            if parser.current().token_type() == &TokenType::CloseCurlyBrace {
                break;
            }
            if parser.current().token_type() == &TokenType::Eof {
                return error_parser!(parser, "Missing close curly brace '}'");
            }
            member_func.push(MemberFunction::parse(parser)?);
        }
        return Ok(ImplDeclaration::new(name, member_func));
    }
}

impl ImplDeclaration {
    pub fn new(name: TypeSpecifier, member_functions: Vec<MemberFunction>) -> Self {
        Self {
            name,
            member_functions,
        }
    }
}

impl AstParse for MemberFunction {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let prototype = MemberFunctionPrototype::parse(parser)?;
        let body = BlockStmt::parse(parser)?;
        return Ok(MemberFunction::new(prototype, body));
    }
}

impl MemberFunction {
    pub fn new(prototype: MemberFunctionPrototype, body: BlockStmt) -> Self {
        Self { prototype, body }
    }

    fn parse_method(parser: &mut Parser) -> Result<Option<VariableType>> {
        if parser.next().token_type() != &TokenType::OpenParen {
            return Ok(None);
        }
        parser.next_token()?; // Skip fn keyword
        parser.next_token()?; // Skip open parenthesis
        let variable_type = VariableType::parse(parser)?;
        parser.next_token_and_expect(TokenType::CloseParen)?;
        return Ok(Some(variable_type));
    }
}

impl AstParse for MemberFunctionPrototype {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let access_specifier = AccessSpecifier::parse(parser)?;
        parser.expect(TokenType::Fn)?;
        let method = MemberFunction::parse_method(parser)?;
        let name = Identifier::parse(parser)?;
        let params = FnDeclaration::parse_params(parser)?;
        let return_type = FnDeclaration::parse_return_type(parser)?;
        return Ok(Self::new(
            access_specifier,
            method,
            name,
            params,
            return_type,
        ));
    }
}

impl MemberFunctionPrototype {
    pub fn new(
        access_specifier: AccessSpecifier,
        method: Option<VariableType>,
        name: Identifier,
        params: Vec<VariableType>,
        return_type: TypeSpecifier,
    ) -> Self {
        Self {
            access_specifier,
            method,
            name,
            params,
            return_type,
        }
    }
}
