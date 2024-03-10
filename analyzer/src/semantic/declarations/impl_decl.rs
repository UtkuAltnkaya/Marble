use std::collections::HashMap;

use crate::{
    ast::{
        declarations::impl_decl::{ImplDeclaration, MemberFunction},
        statements::Statements,
        type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::{
        symbol::{
            data::{Access, SymbolData},
            iter::ToIter,
            node::NodeTypes,
            NodeCallBack, SymbolNode, SymbolNodeRef,
        },
        ToSymbol,
    },
};

impl AstAnalyze for ImplDeclaration {
    fn analyze(&mut self, _parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let struct_name = self.name.to_symbol()?;
        let struct_sym = root
            .iter()
            .struct_sym(struct_name)
            .ok_or(CompilerError::Semantic("Struct not found".to_owned()))?
            .find();

        for member_fn in self.member_functions.iter_mut() {
            member_fn.analyze(struct_sym.clone(), root.clone())?;
        }

        return Ok(TypeSpecifier::Void);
    }
}

impl AstAnalyze for MemberFunction {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let fn_node: SymbolNodeRef = parent
            .iter()
            .function(self.prototype.name.as_ref())
            .ok_or_else(|| CompilerError::Semantic("Function not found".to_owned()))?
            .find();

        self.body.analyze(fn_node, root)?;

        if self.prototype.return_type == TypeSpecifier::Void {
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
            let node = SymbolNode::from((
                method,
                Access::Local,
                member_symbol.clone(),
                Box::new(|var| NodeTypes::Variable(var)) as NodeCallBack,
            ));
            member_symbol.borrow_mut().append(node.into());
        }

        for param in self.prototype.params.iter() {
            let param_symbol = SymbolNode::from((
                param,
                Access::Local,
                member_symbol.clone(),
                Box::new(|var| NodeTypes::Variable(var)) as NodeCallBack,
            ));
            member_symbol.borrow_mut().append(param_symbol.into());
        }

        root.borrow_mut().append(member_symbol);
        return Ok(());
    }
}
