use std::collections::HashMap;

use crate::{
    ast::declarations::impl_decl::{ImplDeclaration, MemberFunction},
    error::{CompilerError, Result},
    symbol_table::{
        symbol::{
            data::{Access, SymbolData},
            iter::ToIter,
            SymbolNode, SymbolNodeRef,
        },
        ToSymbol,
    },
};

impl ToSymbol for ImplDeclaration {
    fn to_symbol(&self, root: SymbolNodeRef) -> Result<()> {
        let struct_name = self.name.to_symbol()?;
        let struct_sym = root
            .iter()
            .struct_sym(struct_name)
            .ok_or(CompilerError::Semantic("Struct not found".to_owned()))?
            .find();

        for member_fn in self.member_functions.iter() {
            member_fn.to_symbol(struct_sym.clone())?;
        }
        return Ok(());
    }
}

impl ToSymbol for MemberFunction {
    fn to_symbol(&self, root: SymbolNodeRef) -> Result<()> {
        let member_data = SymbolData::from((
            self.prototype.name.as_ref(),
            &self.prototype.access_specifier,
            &self.prototype.return_type,
            &self.prototype.params,
        ));
        let member_symbol: SymbolNodeRef =
            SymbolNode::new(member_data, Some(root.clone()), HashMap::new()).into();

        if let Some(method) = &self.prototype.method {
            member_symbol
                .borrow_mut()
                .append(SymbolNode::from((method, Access::Local, member_symbol.clone())).into());
        }

        root.borrow_mut().append(member_symbol);
        return Ok(());
    }
}
