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
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        match self {
            Declarations::Fn(fn_decl) => fn_decl.analyze(parent, root),
            Declarations::Impl(impl_decl) => impl_decl.analyze(parent, root),
            _ => Ok(TypeSpecifier::Void),
        }
    }
}
