pub mod fn_symbol;
pub mod scope;
pub mod struct_symbol;
pub mod symbol_node;
pub mod variable_symbol;

use std::{cell::RefCell, collections::HashMap, rc::Rc};

use self::symbol_node::SymbolNode;

#[allow(dead_code)]
#[derive(Debug)]
pub struct SymbolTable {
    table: HashMap<String, Vec<SymbolNode>>,
}

pub type SymbolTableP = Rc<RefCell<SymbolTable>>;

impl SymbolTable {
    pub fn new() -> Self {
        Self {
            table: HashMap::new(),
        }
    }

    pub fn insert_symbol(&mut self, symbol: &str) {
        self.table.insert(symbol.to_owned(), Vec::new());
    }

    pub fn insert_node(&mut self, symbol: &str, node: SymbolNode) {
        if let Some(symbol_node) = self.table.get_mut(symbol) {
            symbol_node.push(node);
        }
    }
}
