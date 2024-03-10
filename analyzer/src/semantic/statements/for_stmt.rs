use std::collections::HashMap;

use crate::{
    ast::{
        statements::for_stmt::{ForStmt, VariableInit},
        type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::{
        data::{Access, SymbolData},
        node::NodeTypes,
        SymbolNode, SymbolNodeRef,
    },
};

impl AstAnalyze for ForStmt {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let for_node = self.create_symbol(parent.clone())?;

        match &mut self.init {
            VariableInit::Let(let_stmt) => {
                let_stmt.analyze(for_node.clone(), root.clone())?;
            }
            VariableInit::Assign(expr) => {
                expr.analyze(parent.clone(), root.clone())?;
            }
        }
        let condition_type = self.condition.analyze(for_node.clone(), root.clone())?;

        if condition_type != TypeSpecifier::Bool {
            return Err(CompilerError::Semantic(String::from(
                "Condition type must be boolean",
            )));
        }
        // TODO Warn
        self.increment.analyze(for_node.clone(), root.clone())?;

        self.block.analyze(for_node, root)?;

        return Ok(TypeSpecifier::Void);
    }
}

impl ForStmt {
    fn create_symbol(&mut self, parent: SymbolNodeRef) -> Result<SymbolNodeRef> {
        let data = SymbolData::new("for_1".to_owned(), Access::Local, NodeTypes::Block);
        let symbol: SymbolNodeRef =
            SymbolNode::new(data, Some(parent.clone()), HashMap::new()).into();

        parent.borrow_mut().append(symbol.clone());
        return Ok(symbol);
    }
}
