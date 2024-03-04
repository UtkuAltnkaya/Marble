use crate::{
    ast::{
        declarations::fn_decl::FnDeclaration, statements::Statements, type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::{
        symbol::{
            data::Access, iter::ToIter, node::NodeTypes, NodeCallBack, SymbolNode, SymbolNodeRef,
        },
        ToSymbol,
    },
};

impl AstAnalyze for FnDeclaration {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let fn_symbol: SymbolNodeRef = parent
            .iter()
            .function(self.fn_name.as_ref())
            .ok_or_else(|| CompilerError::Semantic(String::from("Function not found")))?
            .find();

        self.body.analyze(fn_symbol, root)?;

        if self.return_type == TypeSpecifier::Void {
            return Ok(TypeSpecifier::Void);
        }

        let return_stmt = self
            .body
            .stmts
            .last()
            .ok_or_else(|| CompilerError::Semantic(String::from("Statement expected")))?;

        if let Statements::Return(_) = return_stmt {
            return Ok(TypeSpecifier::Void);
        }
        return Err(CompilerError::Semantic(String::from(
            "Return Statement Expected",
        )));
    }
}

impl ToSymbol for FnDeclaration {
    fn to_symbol(&self, root: SymbolNodeRef) -> Result<()> {
        let fn_symbol: SymbolNodeRef = SymbolNode::from((self, root.clone())).into();
        for param in self.params.iter() {
            let param_symbol = SymbolNode::from((
                param,
                Access::Local,
                fn_symbol.clone(),
                Box::new(|var| NodeTypes::Variable(var)) as NodeCallBack,
            ));
            fn_symbol.borrow_mut().append(param_symbol.into());
        }
        root.borrow_mut().append(fn_symbol);
        return Ok(());
    }
}
