use crate::{
    ast::{
        statements::{block_stmt::BlockStmt, Statements},
        type_specifier::TypeSpecifier,
    },
    error::Result,
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for BlockStmt {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        for stmt in self.stmts.iter_mut() {
            if let Statements::Block(_) = stmt {
            } else {
                stmt.analyze(parent.clone(), root.clone())?;
            }
        }
        return Ok(TypeSpecifier::Void);
    }
    // add code here
}
