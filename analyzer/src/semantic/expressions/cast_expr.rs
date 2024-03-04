use crate::{
    ast::{expressions::cast_expr::CastExpression, type_specifier::TypeSpecifier},
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for CastExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let type_specifier = self.expression.analyze(parent, root)?;
        if !type_specifier.is_primitive() {
            return Err(CompilerError::Semantic(String::from(
                "Cannot cast the complex type",
            )));
        }
        return Ok(self.target_type.clone());
    }
}
