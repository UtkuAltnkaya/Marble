use crate::{
    ast::{
        declarations::{
            fn_decl::FnDeclaration,
            impl_decl::{ImplDeclaration, MemberFunction},
        },
        type_specifier::TypeSpecifier,
    },
    error::Result,
};

use super::{
    symbol_node::{Access, Scope, Symbol, SymbolNode},
    SymbolTable,
};

#[derive(Debug)]
pub struct FnSymbol {
    pub access: Access,
    pub scope: Scope,
    pub return_type: TypeSpecifier,
    pub params: Vec<TypeSpecifier>,
}

impl From<&FnDeclaration> for FnSymbol {
    fn from(value: &FnDeclaration) -> Self {
        let access = Access::from(&value.access_specifier);
        let scope = Scope::Global;
        let return_type = value.return_type.clone();
        let params: Vec<TypeSpecifier> = value
            .params
            .iter()
            .map(|variable_type| variable_type.type_specifier.clone())
            .collect();
        return Self::new(access, scope, return_type, params);
    }
}

impl Symbol<&FnDeclaration> for FnSymbol {
    fn insert(table: &mut SymbolTable, item: &FnDeclaration) -> Result<()> {
        table.insert_node(
            item.fn_name.as_ref(),
            SymbolNode::Function(FnSymbol::from(item)),
        );
        return Ok(());
    }
}

impl From<(&MemberFunction, &str)> for FnSymbol {
    fn from((value, struct_name): (&MemberFunction, &str)) -> Self {
        let access = Access::from(&value.prototype.access_specifier);
        let scope = Scope::Struct {
            name: struct_name.to_owned(),
            fn_scope: None,
        };
        let return_type = value.prototype.return_type.clone();
        let params: Vec<TypeSpecifier> = value
            .prototype
            .params
            .iter()
            .map(|variable_type| variable_type.type_specifier.clone())
            .collect();
        return Self::new(access, scope, return_type, params);
    }
}

impl Symbol<(&MemberFunction, &str)> for FnSymbol {
    fn insert(table: &mut SymbolTable, (item, symbol): (&MemberFunction, &str)) -> Result<()> {
        table.insert_node(
            item.prototype.name.as_ref(),
            SymbolNode::Function(FnSymbol::from((item, symbol))),
        );
        return Ok(());
    }
}

impl Symbol<&ImplDeclaration> for FnSymbol {
    fn insert(table: &mut SymbolTable, item: &ImplDeclaration) -> Result<()> {
        for member_fn in item.member_functions.iter() {
            FnSymbol::insert(table, (member_fn, item.name.to_symbol()?))?;
        }
        return Ok(());
    }
}

impl FnSymbol {
    pub fn new(
        access: Access,
        scope: Scope,
        return_type: TypeSpecifier,
        params: Vec<TypeSpecifier>,
    ) -> Self {
        Self {
            access,
            scope,
            return_type,
            params,
        }
    }
}
