use crate::{
    ast::declarations::struct_decl::{StructDeclaration, StructField},
    error::Result,
    symbol_table::{
        symbol::{data::Access, node::NodeTypes, NodeCallBack, SymbolNode, SymbolNodeRef},
        ToSymbol,
    },
};

impl ToSymbol for StructDeclaration {
    fn to_symbol(&self, root: SymbolNodeRef) -> Result<()> {
        let struct_symbol: SymbolNodeRef = SymbolNode::from((self, root.clone())).into();
        for field in self.fields.iter() {
            field.to_symbol(struct_symbol.clone())?;
        }
        root.borrow_mut().append(struct_symbol);
        return Ok(());
    }
}

impl ToSymbol for StructField {
    fn to_symbol(&self, root: SymbolNodeRef) -> Result<()> {
        let field = SymbolNode::from((
            &self.field,
            Access::from(&self.access_specifier),
            root.clone(),
            Box::new(|var| NodeTypes::StructField(var)) as NodeCallBack,
        ));
        root.borrow_mut().append(field.into());
        return Ok(());
    }
}
