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

#[allow(unused_variables)]
pub trait AstParse: Sized {
    fn parse(parser: &mut Parser) -> Result<Self>;
}
