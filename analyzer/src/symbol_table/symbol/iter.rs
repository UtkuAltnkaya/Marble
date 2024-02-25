use super::{node::NodeTypes, SymbolNodeRef};

pub struct SymbolIterator {
    node: SymbolNodeRef,
}

pub trait ToIter {
    fn iter(&self) -> SymbolIterator;
}

impl SymbolIterator {
    pub fn new(node: SymbolNodeRef) -> Self {
        Self { node }
    }

    pub fn find(self) -> SymbolNodeRef {
        return self.node.clone();
    }

    pub fn function(&self, name: &str) -> Option<Self> {
        let root = self.node.borrow();
        let node = root.children.get(name)?;
        if let NodeTypes::Function { .. } = &node.borrow().data.node_type {
            return Some(Self::new(node.clone()));
        }
        return None;
    }

    pub fn struct_sym(&self, name: &str) -> Option<Self> {
        let root = self.node.borrow();
        let node = root.children.get(name)?;
        if let NodeTypes::Struct = &node.borrow().data.node_type {
            return Some(Self::new(node.clone()));
        }
        return None;
    }

    pub fn enum_sym(&self, name: &str) -> Option<Self> {
        let root = self.node.borrow();
        let node = root.children.get(name)?;
        if let NodeTypes::Enum = &node.borrow().data.node_type {
            return Some(Self::new(node.clone()));
        }
        return None;
    }

    pub fn variable(&self, name: &str) -> Option<SymbolIterator> {
        let root = self.node.borrow();
        let node = root.children.get(name)?;
        if let NodeTypes::Variable { .. } = node.borrow().data.node_type {
            return Some(Self::new(node.clone()));
        }
        return None;
    }

    pub fn block(&self, name: &str) -> Option<Self> {
        let root = self.node.borrow();
        let node = root.children.get(name)?;
        if let NodeTypes::Block = node.borrow().data.node_type {
            return Some(Self::new(node.clone()));
        }
        return None;
    }
}
