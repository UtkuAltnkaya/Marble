pub mod enum_decl;
pub mod fn_decl;
pub mod impl_decl;
pub mod struct_decl;

use crate::{
    ast::{declarations::Declarations, type_specifier::TypeSpecifier},
    error::Result,
    symbol_table::symbol::SymbolNodeRef,
};

use super::AstAnalyze;

impl AstAnalyze for Declarations {
    fn analyze(&mut self, parent: SymbolNodeRef) -> Result<TypeSpecifier> {
        match self {
            Declarations::Fn(fn_decl) => fn_decl.analyze(parent),
            Declarations::Impl(_) => todo!(),
            _ => Ok(TypeSpecifier::Void),
        }
    }
}
