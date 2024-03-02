use std::{cell::RefCell, rc::Rc};

use crate::{error::Result, lexer::token_type::TokenType, parser::Parser};

use super::{declarations::Declarations, AstParse};

#[derive(Debug)]
pub struct Program {
    pub declarations: Vec<Declarations>,
}

pub type ProgramRef = Rc<RefCell<Program>>;

impl Program {
    pub fn new(declarations: Vec<Declarations>) -> Self {
        Self { declarations }
    }
}

impl AstParse for Program {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let mut decls = Vec::new();
        while parser.current().token_type() != &TokenType::Eof {
            decls.push(Declarations::parse(parser)?);
            parser.next_token()?;
        }
        return Ok(Program::new(decls));
    }
}
