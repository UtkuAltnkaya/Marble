use crate::{ast::declarations::struct_decl::StructDeclaration, error::Result};

use super::{
    symbol_node::{Access, Scope, Symbol, SymbolNode},
    variable_symbol::VariableSymbol,
    SymbolTable,
};

#[derive(Debug)]
pub struct StructSymbol {
    pub access: Access,
    pub scope: Scope,
}

impl From<&StructDeclaration> for StructSymbol {
    fn from(value: &StructDeclaration) -> Self {
        let access = Access::from(&value.access_specifier);
        let scope = Scope::Global;
        return Self::new(access, scope);
    }
}

impl Symbol<&StructDeclaration> for StructSymbol {
    fn insert(table: &mut SymbolTable, item: &StructDeclaration) -> Result<()> {
        table.insert_node(
            item.name.as_ref(),
            SymbolNode::Struct(StructSymbol::from(item)),
        );
        for field in item.fields.iter() {
            VariableSymbol::insert(table, (field, item.name.as_ref()))?;
        }

        return Ok(());
    }
}

impl StructSymbol {
    pub fn new(access: Access, scope: Scope) -> Self {
        Self { access, scope }
    }
}
