use crate::{
    ast::{program::Program, type_specifier::TypeSpecifier},
    error::Result,
    symbol_table::symbol::SymbolNodeRef,
};

use super::AstAnalyze;

impl AstAnalyze for Program {
    fn analyze(&mut self, parent: SymbolNodeRef) -> Result<TypeSpecifier> {
        for decl in self.declarations.iter_mut() {
            decl.analyze(parent.clone())?;
        }
        return Ok(TypeSpecifier::Void);
    }
}
