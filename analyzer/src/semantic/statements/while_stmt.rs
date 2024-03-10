use std::collections::HashMap;

use crate::{
    ast::{statements::while_stmt::WhileStmt, type_specifier::TypeSpecifier},
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::{
        data::{Access, SymbolData},
        node::NodeTypes,
        SymbolNode, SymbolNodeRef,
    },
};

impl AstAnalyze for WhileStmt {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let while_node: SymbolNodeRef = self.create_symbol(parent.clone())?;
        let condition_type = self.condition.analyze(parent, root.clone())?;

        if condition_type != TypeSpecifier::Bool {
            return Err(CompilerError::Semantic(String::from(
                "Condition type must be boolean",
            )));
        }

        self.block.analyze(while_node, root)?;
        return Ok(TypeSpecifier::Void);
    }
}

impl WhileStmt {
    fn create_symbol(&self, parent: SymbolNodeRef) -> Result<SymbolNodeRef> {
        let data = SymbolData::new("while_1".to_owned(), Access::Local, NodeTypes::Block);
        let node: SymbolNodeRef =
            SymbolNode::new(data, Some(parent.clone()), HashMap::new()).into();

        parent.borrow_mut().append(node.clone());

        return Ok(node);
    }
}
