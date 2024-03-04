pub mod data;
pub mod iter;
pub mod node;

use crate::{
    ast::{
        access_specifier::AccessSpecifier,
        declarations::{
            enum_decl::EnumDeclaration, fn_decl::FnDeclaration, struct_decl::StructDeclaration,
        },
        identifier::Identifier,
        type_specifier::TypeSpecifier,
        variable_type::VariableType,
    },
    symbol_table::symbol::{data::Access, node::NodeTypes},
};

use self::{
    data::SymbolData,
    iter::{SymbolIterator, ToIter},
    node::VariableNode,
};
use std::{cell::RefCell, collections::HashMap, fmt::Display, rc::Rc};

pub type SymbolNodeRef = Rc<RefCell<SymbolNode>>;

#[derive(Debug)]
pub struct SymbolNode {
    pub data: SymbolData,
    pub parent: Option<SymbolNodeRef>,
    pub children: HashMap<String, SymbolNodeRef>,
}

impl SymbolNode {
    pub fn new(
        data: SymbolData,
        parent: Option<SymbolNodeRef>,
        children: HashMap<String, SymbolNodeRef>,
    ) -> Self {
        Self {
            data,
            parent,
            children,
        }
    }

    pub fn append(&mut self, child: SymbolNodeRef) {
        let key = child.borrow().data.name.to_owned();
        self.children.insert(key, child);
    }

    fn dfs(&self, f: &mut std::fmt::Formatter<'_>, depth: usize) -> std::fmt::Result {
        let indent = "  ".repeat(depth);
        writeln!(f, "{}Name:{}", indent, self.data.name)?;
        writeln!(f, "{}Access:{}", indent, self.data.access)?;
        writeln!(f, "{}NodeType:{:?}", indent, self.data.node_type)?;

        if let Some(parent) = &self.parent {
            writeln!(f, "{}Parent:{}", indent, parent.borrow().data.name)?;
        }

        if self.children.is_empty() {
            write!(f, "{}Children: {{", indent)?;
        } else {
            writeln!(f, "{}Children: {{", indent)?;
        }

        for (_, value) in &self.children {
            value.borrow().dfs(f, depth + 3)?;
        }

        if self.children.is_empty() {
            writeln!(f, "}}\n")?;
        } else {
            writeln!(f, "{}}}\n", indent)?;
        }

        return Ok(());
    }
}

impl ToIter for SymbolNodeRef {
    fn iter(&self) -> SymbolIterator {
        return SymbolIterator::new(self.clone());
    }
}

impl Display for SymbolNode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        return self.dfs(f, 0);
    }
}

impl Into<SymbolNodeRef> for SymbolNode {
    fn into(self) -> SymbolNodeRef {
        return Rc::new(RefCell::new(self));
    }
}

impl From<(&FnDeclaration, SymbolNodeRef)> for SymbolNode {
    fn from((fn_decl, parent): (&FnDeclaration, SymbolNodeRef)) -> Self {
        let data = SymbolData::from((
            fn_decl.fn_name.as_ref(),
            &fn_decl.access_specifier,
            &fn_decl.return_type,
            &fn_decl.params,
        ));
        return SymbolNode::new(data, Some(parent), HashMap::new());
    }
}

impl From<(&StructDeclaration, SymbolNodeRef)> for SymbolNode {
    fn from((struct_decl, parent): (&StructDeclaration, SymbolNodeRef)) -> Self {
        let data = SymbolData::new(
            struct_decl.name.to_string(),
            Access::from(&struct_decl.access_specifier),
            NodeTypes::Struct,
        );
        return SymbolNode::new(data, Some(parent), HashMap::new());
    }
}

impl From<(&EnumDeclaration, SymbolNodeRef)> for SymbolNode {
    fn from((enum_decl, parent): (&EnumDeclaration, SymbolNodeRef)) -> Self {
        let data = SymbolData::new(
            enum_decl.name.to_string(),
            Access::from(&enum_decl.access_specifier),
            NodeTypes::Enum,
        );
        return SymbolNode::new(data, Some(parent), HashMap::new());
    }
}

//Enum Field
impl From<(&Identifier, SymbolNodeRef)> for SymbolNode {
    fn from((enum_field, parent): (&Identifier, SymbolNodeRef)) -> Self {
        let data = SymbolData::new(
            enum_field.to_string(),
            Access::from(AccessSpecifier::Public),
            NodeTypes::EnumItem,
        );
        return SymbolNode::new(data, Some(parent), HashMap::new());
    }
}

pub type NodeCallBack = Box<dyn Fn(VariableNode) -> NodeTypes>;
impl From<(&VariableType, Access, SymbolNodeRef, NodeCallBack)> for SymbolNode {
    fn from(
        (variable_type, access, parent, node_callback): (
            &VariableType,
            Access,
            SymbolNodeRef,
            NodeCallBack,
        ),
    ) -> Self {
        let variable_node = VariableNode::new(variable_type.type_specifier.clone());
        let node_type = node_callback(variable_node);
        let data = SymbolData::new(variable_type.identifier.to_string(), access, node_type);
        return SymbolNode::new(data, Some(parent), HashMap::new());
    }
}

impl From<(&Identifier, &TypeSpecifier, SymbolNodeRef)> for SymbolNode {
    fn from((name, type_specifier, parent): (&Identifier, &TypeSpecifier, SymbolNodeRef)) -> Self {
        let variable_node = VariableNode::new(type_specifier.clone());
        let data = SymbolData::new(
            name.to_string(),
            Access::Local,
            NodeTypes::Variable(variable_node),
        );
        return SymbolNode::new(data, Some(parent), HashMap::new());
    }
}
