use std::collections::HashMap;

use crate::{
    ast::{
        statements::{block_stmt::BlockStmt, Statements},
        type_specifier::TypeSpecifier,
    },
    error::Result,
    semantic::AstAnalyze,
    symbol_table::symbol::{
        data::{Access, SymbolData},
        node::NodeTypes,
        SymbolNode, SymbolNodeRef,
    },
};

impl AstAnalyze for BlockStmt {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        for stmt in self.stmts.iter_mut() {
            if let Statements::Block(block_stmt) = stmt {
                let block_node = block_stmt.create_symbol(parent.clone())?;
                block_stmt.analyze(block_node, root.clone())?;
            } else {
                stmt.analyze(parent.clone(), root.clone())?;
            }
        }
        return Ok(TypeSpecifier::Void);
    }
}

impl BlockStmt {
    fn create_symbol(&self, parent: SymbolNodeRef) -> Result<SymbolNodeRef> {
        let data = SymbolData::new("block_1".to_owned(), Access::Local, NodeTypes::Block);
        let node: SymbolNodeRef =
            SymbolNode::new(data, Some(parent.clone()), HashMap::new()).into();
        parent.borrow_mut().append(node.clone());
        return Ok(node);
    }
}
