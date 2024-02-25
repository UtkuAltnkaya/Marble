use crate::{
    ast::{declarations::fn_decl::FnDeclaration, type_specifier::TypeSpecifier},
    error::Result,
    semantic::AstAnalyze,
    symbol_table::SymbolTable,
};

impl AstAnalyze for FnDeclaration {
    fn analyze(&mut self, _symbol_table: &mut SymbolTable) -> Result<TypeSpecifier> {
        // let scope = Scope::Function(self.fn_name.as_ref().to_owned());
        // for param in self.params.iter() {
        //     VariableSymbol::insert(symbol_table, (param, &scope))?;
        // }
        // self.body.analyze(symbol_table, &scope)?;
        todo!()
    }
}
