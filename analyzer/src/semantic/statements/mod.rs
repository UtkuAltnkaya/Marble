pub mod block_stmt;
pub mod defer_stmt;
pub mod for_stmt;
pub mod if_stmt;
pub mod let_stmt;
pub mod return_stmt;
pub mod while_stmt;

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
            Statements::Defer(defer_stmt) => defer_stmt.analyze(parent, root),
            Statements::For(for_stmt) => for_stmt.analyze(parent, root),
            Statements::While(while_stmt) => while_stmt.analyze(parent, root),
            Statements::If(if_stmt) => if_stmt.analyze(parent, root),
            Statements::Block(block) => block.analyze(parent, root),
            Statements::Expression(expression) => expression.analyze(parent, root),
        }
    }
}
