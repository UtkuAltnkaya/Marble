pub mod fn_decl;

use crate::{
    ast::{declarations::Declarations, type_specifier::TypeSpecifier},
    error::Result,
    symbol_table::SymbolTable,
};

use super::AstAnalyze;

impl AstAnalyze for Declarations {
    fn analyze(&mut self, symbol_table: &mut SymbolTable) -> Result<TypeSpecifier> {
        match self {
            Declarations::Fn(fn_decl) => fn_decl.analyze(symbol_table),
            Declarations::Struct(_) => todo!(),
            Declarations::Enum(_) => todo!(),
            Declarations::Impl(_) => todo!(),
        }
    }
}
