use crate::{
    ast::{statements::return_stmt::ReturnStmt, type_specifier::TypeSpecifier},
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for ReturnStmt {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let binding = parent.clone();
        let binding = binding.borrow();
        let fn_node = binding.data.node_type.function();

        if let Some(value) = self.value.as_mut() {
            let type_specifier = value.analyze(parent, root)?;
            if type_specifier == fn_node.return_type {
                return Ok(TypeSpecifier::Void);
            }
        } else if fn_node.return_type == TypeSpecifier::Void {
            return Ok(TypeSpecifier::Void);
        }

        return Err(CompilerError::Semantic(String::from(
            "Return value and return type of function does not match",
        )));
    }
}
