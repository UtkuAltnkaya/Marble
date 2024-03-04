use crate::{
    ast::{expressions::array_init_expr::ArrayInitExpression, type_specifier::TypeSpecifier},
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for ArrayInitExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let expr_type = self.array[0].analyze(parent.clone(), root.clone())?;
        for i in 1..self.array.len() {
            let type_specifier = self.array[i].analyze(parent.clone(), root.clone())?;
            if expr_type != type_specifier {
                return Err(CompilerError::Semantic(String::from(
                    "Array item types must be same!",
                )));
            }
        }
        return Ok(TypeSpecifier::ArrayType {
            type_specifier: Box::new(expr_type),
            size: self.size,
        });
    }
}
