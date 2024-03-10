use crate::{
    ast::{statements::defer_stmt::DeferStmt, type_specifier::TypeSpecifier},
    error::Result,
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for DeferStmt {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        self.expr.analyze(parent, root)?;
        return Ok(TypeSpecifier::Void);
    }
}
