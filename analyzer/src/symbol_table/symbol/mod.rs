pub mod data;
pub mod iter;
pub mod node;

use crate::{
    ast::{
        declarations::{
            enum_decl::EnumDeclaration, fn_decl::FnDeclaration, impl_decl::ImplDeclaration,
            struct_decl::StructDeclaration,
        },
        variable_type::VariableType,
    },
    error::{CompilerError, Result},
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

    pub fn append_impl(impl_decl: &ImplDeclaration, root: SymbolNodeRef) -> Result<()> {
        let struct_name = impl_decl.name.to_symbol()?; // TODO allow to implement other than user define type
        let struct_sym = root
            .iter()
            .struct_sym(struct_name)
            .ok_or(CompilerError::Semantic("Struct not found".to_owned()))?
            .find();

        for member in impl_decl.member_functions.iter() {
            let data = SymbolData::from((
                member.prototype.name.as_ref(),
                &member.prototype.access_specifier,
                &member.prototype.return_type,
                &member.prototype.params,
            ));
            let node: SymbolNodeRef =
                SymbolNode::new(data, Some(struct_sym.clone()), HashMap::new()).into();

            if let Some(method) = &member.prototype.method {
                node.borrow_mut()
                    .append(SymbolNode::from((method, node.clone())).into());
            }

            struct_sym.borrow_mut().append(node);
        }

        return Ok(());
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

impl From<(&VariableType, SymbolNodeRef)> for SymbolNode {
    fn from((variable_type, parent): (&VariableType, SymbolNodeRef)) -> Self {
        let variable_node = VariableNode::new(variable_type.type_specifier.clone());
        let data = SymbolData::new(
            variable_type.identifier.to_string(),
            Access::Local,
            NodeTypes::Variable(variable_node),
        );
        return SymbolNode::new(data, Some(parent), HashMap::new());
    }
}
