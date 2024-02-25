pub mod symbol;

use std::{cell::RefCell, collections::HashMap, fmt::Display, rc::Rc};

use crate::ast::access_specifier::AccessSpecifier;

use self::symbol::{
    data::{Access, SymbolData},
    iter::{SymbolIterator, ToIter},
    node::NodeTypes,
    SymbolNode, SymbolNodeRef,
};

pub type SymbolTableRef = Rc<RefCell<SymbolTable>>;

#[derive(Debug)]
pub struct SymbolTable {
    root: SymbolNodeRef,
}

impl Display for SymbolTable {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        return writeln!(f, "{}", self.root.borrow());
    }
}

impl SymbolTable {
    pub fn new() -> Self {
        let data = SymbolData::new(
            String::from("Global"),
            Access::from(AccessSpecifier::Public),
            NodeTypes::Global,
        );
        let root = SymbolNode::new(data, None, HashMap::new());
        Self { root: root.into() }
    }

    pub fn root(&self) -> SymbolNodeRef {
        return self.root.clone();
    }
}

impl ToIter for SymbolTableRef {
    fn iter(&self) -> SymbolIterator {
        return SymbolIterator::new(self.borrow().root.clone());
    }
}

impl ToIter for SymbolTable {
    fn iter(&self) -> SymbolIterator {
        return SymbolIterator::new(self.root.clone());
    }
}
