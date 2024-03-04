use crate::{
    ast::{statements::let_stmt::LetStmt, type_specifier::TypeSpecifier},
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::{
        symbol::{SymbolNode, SymbolNodeRef},
        ToSymbol,
    },
};

impl AstAnalyze for LetStmt {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        if let Some(ref mut value) = self.value {
            let type_specifier = value.analyze(parent.clone(), root)?;
            match &self.type_specifier {
                Some(type_info) => {
                    if &type_specifier != type_info {
                        return Err(CompilerError::Semantic(String::from("Miss matched types")));
                    }
                }
                _ => {
                    self.type_specifier = Some(type_specifier);
                }
            }
        }
        self.to_symbol(parent)?;
        return Ok(TypeSpecifier::Void);
    }
}

impl ToSymbol for LetStmt {
    fn to_symbol(&self, root: SymbolNodeRef) -> Result<()> {
        let variable_symbol = SymbolNode::from((
            &self.identifier,
            unsafe { self.type_specifier.as_ref().unwrap_unchecked() },
            root.clone(),
        ));
        root.borrow_mut().append(variable_symbol.into());
        return Ok(());
    }
}
