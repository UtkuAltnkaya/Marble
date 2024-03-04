use crate::{
    ast::{expressions::primitive_expr::PrimitiveExpression, type_specifier::TypeSpecifier},
    error::Result,
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for PrimitiveExpression {
    fn analyze(&mut self, _parent: SymbolNodeRef, _root: SymbolNodeRef) -> Result<TypeSpecifier> {
        return Ok(self.type_specifier.clone());
    }
}
