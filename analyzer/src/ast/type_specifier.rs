use std::{num::ParseIntError, usize};

use crate::{error::Result, error_parser, lexer::token_type::TokenType, parser::Parser};

use super::{identifier::Identifier, AstParse};

#[derive(Debug, Clone)]
pub enum TypeSpecifier {
    Int,
    Usize,
    Float,
    Double,
    Char,
    Str,
    Bool,
    Void,
    UserDefine(Identifier),
    Pointer(Box<TypeSpecifier>),
    ArrayType {
        type_specifier: Box<TypeSpecifier>,
        size: usize,
    },
}

impl AstParse for TypeSpecifier {
    fn parse(parser: &mut Parser) -> Result<Self> {
        if parser.current().token_type() == &TokenType::Identifier {
            return Self::handle_user_define(parser);
        }
        return Self::handle_primitive(parser);
    }
}

impl TypeSpecifier {
    pub fn to_symbol(&self) -> Result<&str> {
        Ok(match self {
            TypeSpecifier::Int => "int",
            TypeSpecifier::Usize => "usize",
            TypeSpecifier::Float => "float",
            TypeSpecifier::Double => "double",
            TypeSpecifier::Char => "char",
            TypeSpecifier::Str => "str",
            TypeSpecifier::Bool => "bool",
            TypeSpecifier::UserDefine(id) => id.as_ref(),
            _ => todo!("Type cannot be symbol"),
        })
    }

    fn handle_primitive(parser: &mut Parser) -> Result<Self> {
        let primitive = Self::get_primitive(parser)?;
        if let Some(complex) = Self::check(parser, &primitive)? {
            return Ok(complex);
        }
        return Ok(primitive);
    }

    fn get_primitive(parser: &mut Parser) -> Result<Self> {
        return Ok(match parser.current().token_type() {
            TokenType::Int => TypeSpecifier::Int,
            TokenType::Usize => TypeSpecifier::Usize,
            TokenType::Float => TypeSpecifier::Float,
            TokenType::Double => TypeSpecifier::Double,
            TokenType::Bool => TypeSpecifier::Bool,
            TokenType::CharKeyword => TypeSpecifier::Char,
            TokenType::Str => TypeSpecifier::Str,
            TokenType::Void => TypeSpecifier::Void,
            _ => return error_parser!(parser, "Unknown TypeSpecifier"),
        });
    }

    fn handle_user_define(parser: &mut Parser) -> Result<Self> {
        let type_specifier = Self::UserDefine(Identifier::from(parser.current()));
        return Ok(match Self::check(parser, &type_specifier)? {
            Some(complex) => complex,
            None => type_specifier,
        });
    }

    fn check(parser: &mut Parser, type_specifier: &TypeSpecifier) -> Result<Option<Self>> {
        if parser.next().token_type() == &TokenType::OpenBracket {
            return Ok(Some(Self::handle_array(parser, type_specifier.clone())?));
        }

        if parser.next().token_type() == &TokenType::Multiply {
            return Ok(Some(Self::handle_pointer(parser, type_specifier.clone())?));
        }
        return Ok(None);
    }

    fn handle_array(parser: &mut Parser, type_specifier: TypeSpecifier) -> Result<Self> {
        parser.next_token()?;
        parser.next_token()?;
        let size: usize = parser
            .current()
            .text()
            .parse()
            .or_else(|err: ParseIntError| error_parser!(parser, err.to_string()))?;

        parser.next_token_and_expect(TokenType::CloseBracket)?;
        return Ok(TypeSpecifier::ArrayType {
            type_specifier: Box::from(type_specifier),
            size,
        });
    }

    fn handle_pointer(parser: &mut Parser, type_specifier: TypeSpecifier) -> Result<Self> {
        if parser.next().token_type() != &TokenType::Multiply {
            return Ok(type_specifier);
        }
        let type_specifier = TypeSpecifier::Pointer(Box::from(type_specifier));
        parser.next_token()?;
        return Self::handle_pointer(parser, type_specifier);
    }
}
