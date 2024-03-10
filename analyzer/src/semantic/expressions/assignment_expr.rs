use crate::{
    ast::{
        expressions::{assignment_expr::AssignmentExpression, Expression},
        type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for AssignmentExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let left = self.analyze_left(parent.clone(), root.clone())?;
        if let TypeSpecifier::ArrayType { .. } = &left {
            return Err(CompilerError::Semantic(String::from(
                "Cannot assign to array",
            )));
        }
        let right = self.value.analyze(parent, root)?;

        if left != right {
            return Err(CompilerError::Semantic(String::from(
                "Left and Right types are not matched",
            )));
        }
        return Ok(left);
    }
}

impl AssignmentExpression {
    fn analyze_left(
        &mut self,
        parent: SymbolNodeRef,
        root: SymbolNodeRef,
    ) -> Result<TypeSpecifier> {
        let result = matches!(
            self.variable.as_ref(),
            Expression::Binary(_)
                | Expression::FnCall(_)
                | Expression::ArrayInit(_)
                | Expression::ObjectInit(_)
                | Expression::Primitive(_)
        );

        if result {
            return Err(CompilerError::Semantic(String::from(
                "Left hand side expression is not valid",
            )));
        }

        return self.variable.analyze(parent, root);
    }
}
