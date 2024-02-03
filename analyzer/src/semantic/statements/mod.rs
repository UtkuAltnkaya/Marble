pub mod block_stmt;

use crate::{
    ast::{statements::Statements, type_specifier::TypeSpecifier},
    error::Result,
    symbol_table::{symbol_node::Scope, SymbolTable},
};

use super::AstAnalyze;

impl AstAnalyze for Statements {
    fn analyze(&mut self, symbol_table: &mut SymbolTable, scope: &Scope) -> Result<TypeSpecifier> {
        match self {
            Statements::Let(_) => todo!(),
            Statements::Return(_) => todo!(),
            Statements::Defer(_) => todo!(),
            Statements::For(_) => todo!(),
            Statements::While(_) => todo!(),
            Statements::If(_) => todo!(),
            Statements::Block(block) => block.analyze(symbol_table, scope),
            Statements::Expression(_) => todo!(),
        }
    }
}
