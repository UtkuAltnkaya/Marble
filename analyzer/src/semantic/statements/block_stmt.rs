use crate::{
    ast::{
        statements::{block_stmt::BlockStmt, Statements},
        type_specifier::TypeSpecifier,
    },
    error::Result,
    semantic::AstAnalyze,
    symbol_table::SymbolTable,
};

impl AstAnalyze for BlockStmt {
    fn analyze(&mut self, symbol_table: &mut SymbolTable) -> Result<TypeSpecifier> {
        for stmt in self.stmts.iter_mut() {
            if let Statements::Block(_) = stmt {
            } else {
                stmt.analyze(symbol_table)?;
            }
        }
        return Ok(TypeSpecifier::Void);
    }
    // add code here
}
