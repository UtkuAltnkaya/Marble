use std::fmt::Display;

use crate::ast::{
    access_specifier::AccessSpecifier, type_specifier::TypeSpecifier, variable_type::VariableType,
};

use super::node::{FunctionNode, NodeTypes};

#[derive(Debug)]
pub struct SymbolData {
    pub name: String,
    pub access: Access,
    pub node_type: NodeTypes,
}

#[derive(Debug)]
pub enum Access {
    Local,
    AccessSpecifier(AccessSpecifier),
}

impl SymbolData {
    pub fn new(name: String, access: Access, node_type: NodeTypes) -> Self {
        Self {
            name,
            access,
            node_type,
        }
    }
}

impl From<(&str, &AccessSpecifier, &TypeSpecifier, &Vec<VariableType>)> for SymbolData {
    fn from(
        (name, access_specifier, return_type, params): (
            &str,
            &AccessSpecifier,
            &TypeSpecifier,
            &Vec<VariableType>,
        ),
    ) -> Self {
        let func_node = FunctionNode::new(
            return_type.clone(),
            params
                .iter()
                .map(|variable_type| variable_type.type_specifier.clone())
                .collect(),
        );
        return SymbolData::new(
            name.to_owned(),
            Access::from(access_specifier),
            NodeTypes::Function(func_node),
        );
    }
}

impl From<&AccessSpecifier> for Access {
    fn from(value: &AccessSpecifier) -> Self {
        return Self::AccessSpecifier(value.clone());
    }
}

impl From<AccessSpecifier> for Access {
    fn from(value: AccessSpecifier) -> Self {
        return Self::AccessSpecifier(value);
    }
}

impl Display for Access {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let display = match self {
            Access::Local => "Local",
            Access::AccessSpecifier(access_specifier) => match access_specifier {
                AccessSpecifier::Public => "Public",
                AccessSpecifier::Private => "Private",
            },
        };
        return write!(f, "{}", display);
    }
}
