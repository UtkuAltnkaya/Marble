pub mod declarations;
pub mod expressions;
pub mod program;
pub mod statements;

use crate::{
    ast::{program::ProgramRef, type_specifier::TypeSpecifier},
    error::Result,
    symbol_table::{symbol::SymbolNodeRef, SymbolTableRef},
};

pub struct SemanticAnalyzer {
    symbol_table: SymbolTableRef,
    program: ProgramRef,
}

pub trait AstAnalyze: Sized {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier>;
}

impl SemanticAnalyzer {
    pub fn new(symbol_table: SymbolTableRef, program: ProgramRef) -> Self {
        Self {
            symbol_table,
            program,
        }
    }

    pub fn analyze(&mut self) -> Result<()> {
        let mut program = self.program.borrow_mut();
        program.analyze(
            self.symbol_table.borrow().root(),
            self.symbol_table.borrow().root(),
        )?;
        return Ok(());
    }
}
