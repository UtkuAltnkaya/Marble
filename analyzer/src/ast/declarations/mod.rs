pub mod enum_decl;
pub mod fn_decl;
pub mod impl_decl;
pub mod struct_decl;

use self::{
    enum_decl::EnumDeclaration, fn_decl::FnDeclaration, impl_decl::ImplDeclaration,
    struct_decl::StructDeclaration,
};

use super::{access_specifier::AccessSpecifier, AstParse};
use crate::{error::CompilerError, error_parser, lexer::token_type::TokenType, parser::Parser};

#[derive(Debug)]
pub enum Declarations {
    Fn(FnDeclaration),
    Struct(StructDeclaration),
    Enum(EnumDeclaration),
    Impl(ImplDeclaration),
}

impl AstParse for Declarations {
    fn parse(parser: &mut Parser) -> Result<Self, CompilerError> {
        let access_specifier = AccessSpecifier::parse(parser)?;
        match parser.current().token_type() {
            TokenType::Fn => {
                let mut fn_decl = FnDeclaration::parse(parser)?;
                fn_decl.access_specifier = access_specifier;
                return Ok(Declarations::Fn(fn_decl));
            }
            TokenType::Struct => {
                let mut struct_decl = StructDeclaration::parse(parser)?;
                struct_decl.access_specifier = access_specifier;
                return Ok(Declarations::Struct(struct_decl));
            }
            TokenType::Enum => {
                let mut enum_decl = EnumDeclaration::parse(parser)?;
                enum_decl.access_specifier = access_specifier;
                return Ok(Declarations::Enum(enum_decl));
            }
            TokenType::Impl => {
                if let AccessSpecifier::Public = access_specifier {
                    return error_parser!(parser, "Unexpected access specifier");
                }
                return Ok(Declarations::Impl(ImplDeclaration::parse(parser)?));
            }
            _ => return error_parser!(parser, "Unknown declarations"),
        }
    }
}
