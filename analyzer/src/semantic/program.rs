use crate::{
    ast::{program::Program, type_specifier::TypeSpecifier},
    error::Result,
    symbol_table::SymbolTable,
};

use super::AstAnalyze;

impl AstAnalyze for Program {
    fn analyze(&mut self, symbol_table: &mut SymbolTable) -> Result<TypeSpecifier> {
        for decl in self.declarations.iter_mut() {
            decl.analyze(symbol_table)?;
        }
        return Ok(TypeSpecifier::Void);
    }
}
