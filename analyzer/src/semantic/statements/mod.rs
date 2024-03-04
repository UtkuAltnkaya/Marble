pub mod block_stmt;
pub mod let_stmt;
pub mod return_stmt;

use crate::{
    ast::{statements::Statements, type_specifier::TypeSpecifier},
    error::Result,
    symbol_table::symbol::SymbolNodeRef,
};

use super::AstAnalyze;

impl AstAnalyze for Statements {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        match self {
            Statements::Let(let_stmt) => let_stmt.analyze(parent, root),
            Statements::Return(return_stmt) => return_stmt.analyze(parent, root),
            Statements::Defer(_) => todo!(),
            Statements::For(_) => todo!(),
            Statements::While(_) => todo!(),
            Statements::If(_) => todo!(),
            Statements::Block(block) => block.analyze(parent, root),
            Statements::Expression(expression) => expression.analyze(parent, root),
        }
    }
}
