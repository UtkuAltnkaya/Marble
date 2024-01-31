pub mod access_specifier;
pub mod declarations;
pub mod expressions;
pub mod identifier;
pub mod parenthesis;
pub mod program;
pub mod statements;
pub mod type_specifier;
pub mod variable_type;

use crate::{error::Result, parser::Parser};

// use self::type_specifier::TypeSpecifier;

#[allow(unused_variables)]
pub trait AstNode: Sized {
    fn parse(parser: &mut Parser) -> Result<Self> {
        unreachable!("Unreachable")
    }
    // fn analyze() -> Result<TypeSpecifier>;
}
