use crate::{
    ast::{expressions::fn_call_expr::FnCallExpression, type_specifier::TypeSpecifier},
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::{iter::ToIter, node::FunctionNode, SymbolNodeRef},
};

impl AstAnalyze for FnCallExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let fn_symbol = self.fn_function(root.clone())?;

        let binding = fn_symbol.borrow();
        let fn_node = binding.data.node_type.function();

        self.check_params(fn_node)?;
        self.check_params_type(fn_node, parent, root)?;

        return Ok(fn_node.return_type.clone());
    }
}

impl FnCallExpression {
    fn fn_function(&self, root: SymbolNodeRef) -> Result<SymbolNodeRef> {
        let fn_name = self.fn_name.identifier()?;

        let mut iter = root.iter();
        if let Some(namespace) = &self.namespace {
            let struct_name = namespace.name.identifier()?;
            iter = iter
                .struct_sym(struct_name.id.as_ref())
                .ok_or_else(|| CompilerError::Semantic(String::from("Cannot find the struct")))?;
        }

        let fn_symbol = iter
            .function(fn_name.id.as_ref())
            .ok_or_else(|| CompilerError::Semantic("Cannot find the function".to_owned()))?
            .find();

        return Ok(fn_symbol);
    }

    fn check_params(&self, fn_node: &FunctionNode) -> Result<()> {
        if fn_node.params.len() < self.args.len() {
            return Err(CompilerError::Semantic(String::from("Too many parameter")));
        }
        if fn_node.params.len() > self.args.len() {
            return Err(CompilerError::Semantic(String::from("Missing parameter")));
        }
        return Ok(());
    }

    fn check_params_type(
        &mut self,
        fn_node: &FunctionNode,
        parent: SymbolNodeRef,
        root: SymbolNodeRef,
    ) -> Result<()> {
        for i in 0..self.args.len() {
            let param_type = self.args[i].analyze(parent.clone(), root.clone())?;
            if param_type != fn_node.params[i] {
                return Err(CompilerError::Semantic(String::from(
                    "Parameter expression type does not match",
                )));
            }
        }

        return Ok(());
    }
}
