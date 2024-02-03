pub mod declarations;
pub mod program;
pub mod statements;

use crate::{
    ast::{declarations::Declarations, program::ProgramP, type_specifier::TypeSpecifier},
    error::Result,
    symbol_table::{
        fn_symbol::FnSymbol,
        struct_symbol::StructSymbol,
        symbol_node::{Scope, Symbol},
        SymbolTable, SymbolTableP,
    },
};

#[allow(dead_code)]
pub struct SemanticAnalyzer {
    symbol_table: SymbolTableP,
    program: ProgramP,
}

pub trait AstAnalyze: Sized {
    fn analyze(&mut self, symbol_table: &mut SymbolTable, scope: &Scope) -> Result<TypeSpecifier>;
}

impl SemanticAnalyzer {
    pub fn new(symbol_table: SymbolTableP, program: ProgramP) -> Self {
        Self {
            symbol_table,
            program,
        }
    }

    pub fn analyze(&mut self) -> Result<()> {
        self.init_symbol_table()?;
        let mut symbol_table = self.symbol_table.borrow_mut();
        let mut program = self.program.borrow_mut();
        program.analyze(&mut symbol_table, &Scope::Global)?;
        return Ok(());
    }

    fn init_symbol_table(&mut self) -> Result<()> {
        let mut table = self.symbol_table.borrow_mut();
        for decl in self.program.borrow().declarations.iter() {
            match decl {
                Declarations::Fn(fn_decl) => FnSymbol::insert(&mut table, fn_decl)?,
                Declarations::Struct(struct_decl) => StructSymbol::insert(&mut table, struct_decl)?,
                Declarations::Enum(_) => continue,
                Declarations::Impl(impl_decl) => FnSymbol::insert(&mut table, impl_decl)?,
            }
        }
        return Ok(());
    }
}
