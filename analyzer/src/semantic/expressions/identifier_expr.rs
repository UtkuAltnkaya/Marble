use crate::{
    ast::{
        expressions::identifier_expression::IdentifierExpression, type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::{iter::ToIter, SymbolNodeRef},
};

impl AstAnalyze for IdentifierExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let node_iter = parent.iter();
        let node = node_iter.variable(self.id.as_ref());

        match node {
            Some(var) => {
                let var = var.find();
                let node_types = &var.borrow().data.node_type;
                let var = node_types.variable();
                return Ok(var.type_specifier.clone());
            }
            None => {
                let parent: SymbolNodeRef = node_iter
                    .parent()
                    .ok_or_else(|| CompilerError::Semantic("Cannot find the variable".to_owned()))?
                    .find();
                return self.analyze(parent, root);
            }
        }
    }
}
