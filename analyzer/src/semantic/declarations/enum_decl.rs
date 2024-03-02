use crate::{
    ast::declarations::enum_decl::EnumDeclaration,
    error::Result,
    symbol_table::{
        symbol::{SymbolNode, SymbolNodeRef},
        ToSymbol,
    },
};

impl ToSymbol for EnumDeclaration {
    fn to_symbol(&self, root: SymbolNodeRef) -> Result<()> {
        let enum_symbol: SymbolNodeRef = SymbolNode::from((self, root.clone())).into();
        for field in self.fields.iter() {
            let field_symbol = SymbolNode::from((field, enum_symbol.clone()));
            enum_symbol.borrow_mut().append(field_symbol.into());
        }

        root.borrow_mut().append(enum_symbol);
        return Ok(());
    }
}
