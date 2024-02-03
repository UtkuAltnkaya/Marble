use crate::{ast::access_specifier::AccessSpecifier, error::Result};

use super::{
    fn_symbol::FnSymbol, struct_symbol::StructSymbol, variable_symbol::VariableSymbol, SymbolTable,
};

#[derive(Debug)]
pub enum SymbolNode {
    Function(FnSymbol),
    Struct(StructSymbol),
    Variable(VariableSymbol),
    Method(),
}

pub trait Symbol<T> {
    fn insert(table: &mut SymbolTable, item: T) -> Result<()>;
}

#[derive(Debug, Clone)]
pub enum Scope {
    Global,
    Function(String),
    Struct {
        name: String,
        fn_scope: Option<Box<Scope>>,
    },
    Enum(),
    Block {
        parent: Box<Scope>,
        name: String,
        scope: Box<Scope>,
    },
}

#[derive(Debug)]
pub enum Access {
    AccessSpecifier(AccessSpecifier),
    Local,
}

impl From<&AccessSpecifier> for Access {
    fn from(value: &AccessSpecifier) -> Self {
        Access::AccessSpecifier(value.clone())
    }
}
