use std::collections::HashMap;

use crate::{
    ast::{
        statements::if_stmt::{ElseIfStmt, IfStmt},
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

impl AstAnalyze for IfStmt {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let condition_type = self.condition.analyze(parent.clone(), root.clone())?;

        if condition_type != TypeSpecifier::Bool {
            return Err(CompilerError::Semantic(String::from(
                "Condition type must be boolean",
            )));
        }

        let if_node: SymbolNodeRef = Self::create_symbol("if_1", parent.clone())?;
        self.block.analyze(if_node, root.clone())?;

        for else_if in self.else_ifs.iter_mut() {
            else_if.analyze(parent.clone(), root.clone())?;
        }

        if let Some(else_stmt) = self.else_stmt.as_mut() {
            let else_node = IfStmt::create_symbol("else_1", parent)?;
            else_stmt.analyze(else_node, root)?;
        }

        return Ok(TypeSpecifier::Void);
    }
}

impl AstAnalyze for ElseIfStmt {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let condition_type = self.condition.analyze(parent.clone(), root.clone())?;

        if condition_type != TypeSpecifier::Bool {
            return Err(CompilerError::Semantic(String::from(
                "Condition type must be boolean",
            )));
        }

        let else_if_node = IfStmt::create_symbol("else_if_1", parent)?;

        self.block.analyze(else_if_node, root)?;

        return Ok(TypeSpecifier::Void);
    }
}

impl IfStmt {
    fn create_symbol(name: &str, parent: SymbolNodeRef) -> Result<SymbolNodeRef> {
        let data = SymbolData::new(name.to_owned(), Access::Local, NodeTypes::Block);
        let symbol: SymbolNodeRef =
            SymbolNode::new(data, Some(parent.clone()), HashMap::new()).into();
        parent.borrow_mut().append(symbol.clone());
        return Ok(symbol);
    }
}
