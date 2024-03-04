use crate::{
    ast::{expressions::binary_expr::BinaryExpression, type_specifier::TypeSpecifier},
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for BinaryExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let left_type = self.left.analyze(parent.clone(), root.clone())?;
        let right_type = self.right.analyze(parent, root)?;

        if left_type != right_type {
            return Err(CompilerError::Semantic(String::from(
                "Left and Right hand-side must be the same type",
            )));
        }

        if !left_type.is_primitive() {
            return Err(CompilerError::Semantic(String::from(
                "Cannot apply binary operation to complex type",
            )));
        }

        return Ok(left_type);
    }
}
