use super::{node::NodeTypes, SymbolNodeRef};

pub struct SymbolIterator {
    node: SymbolNodeRef,
}

#[derive(PartialEq)]
pub enum IterNodeTypes {
    Global,
    Variable,
    Function,
    Block,
    Struct,
    StructField,
    Enum,
    EnumItem,
}

impl From<&NodeTypes> for IterNodeTypes {
    fn from(value: &NodeTypes) -> Self {
        match value {
            NodeTypes::Variable(_) => IterNodeTypes::Variable,
            NodeTypes::Function(_) => IterNodeTypes::Function,
            NodeTypes::Block => IterNodeTypes::Block,
            NodeTypes::Global => IterNodeTypes::Global,
            NodeTypes::Struct => IterNodeTypes::Struct,
            NodeTypes::StructField(_) => IterNodeTypes::StructField,
            NodeTypes::Enum => IterNodeTypes::Enum,
            NodeTypes::EnumItem => IterNodeTypes::EnumItem,
        }
    }
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

    pub fn parent(&self) -> Option<Self> {
        let root = self.node.borrow();
        let node = &root.parent;
        if let Some(node) = node {
            return Some(Self::new(node.clone()));
        }
        return None;
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

    pub fn struct_field(&self, name: &str) -> Option<SymbolIterator> {
        let root = self.node.borrow();
        let node = root.children.get(name)?;
        if let NodeTypes::StructField { .. } = node.borrow().data.node_type {
            return Some(Self::new(node.clone()));
        }
        return None;
    }

    pub fn count(&self, filter: IterNodeTypes) -> usize {
        return self
            .node
            .borrow()
            .children
            .values()
            .filter(|value| IterNodeTypes::from(&value.borrow().data.node_type) == filter)
            .count();
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
