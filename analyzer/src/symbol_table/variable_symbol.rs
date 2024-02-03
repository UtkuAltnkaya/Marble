use crate::{
    ast::{
        declarations::struct_decl::StructField, type_specifier::TypeSpecifier,
        variable_type::VariableType,
    },
    error::Result,
    symbol_table::symbol_node::SymbolNode,
};

use super::{
    symbol_node::{Access, Scope, Symbol},
    SymbolTable,
};

#[derive(Debug)]
pub struct VariableSymbol {
    pub type_specifier: TypeSpecifier,
    pub scope: Scope,
    pub access: Access,
}

impl From<(&StructField, &str)> for VariableSymbol {
    fn from((value, scope_name): (&StructField, &str)) -> Self {
        let type_specifier = value.field.type_specifier.clone();
        let access = Access::from(&value.access_specifier);
        let scope = Scope::Struct {
            name: scope_name.to_owned(),
            fn_scope: None,
        };
        return Self::new(type_specifier, scope, access);
    }
}

impl Symbol<(&StructField, &str)> for VariableSymbol {
    fn insert(table: &mut SymbolTable, (item, scope_name): (&StructField, &str)) -> Result<()> {
        table.insert_node(
            item.field.identifier.as_ref(),
            SymbolNode::Variable(VariableSymbol::from((item, scope_name))),
        );
        return Ok(());
    }
}

impl From<(&VariableType, &Scope)> for VariableSymbol {
    fn from((value, scope): (&VariableType, &Scope)) -> Self {
        let type_specifier = value.type_specifier.clone();
        let access = Access::Local;
        let scope = scope.clone();
        return Self::new(type_specifier, scope, access);
    }
}

impl Symbol<(&VariableType, &Scope)> for VariableSymbol {
    fn insert(table: &mut SymbolTable, (item, scope): (&VariableType, &Scope)) -> Result<()> {
        table.insert_node(
            item.identifier.as_ref(),
            SymbolNode::Variable(VariableSymbol::from((item, scope))),
        );
        return Ok(());
    }
}

impl VariableSymbol {
    pub fn new(type_specifier: TypeSpecifier, scope: Scope, access: Access) -> Self {
        Self {
            type_specifier,
            scope,
            access,
        }
    }
}
