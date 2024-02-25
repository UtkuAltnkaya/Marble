pub mod declarations;
pub mod program;
pub mod statements;

use crate::{
    ast::{declarations::Declarations, program::ProgramP, type_specifier::TypeSpecifier},
    error::Result,
    symbol_table::{
        symbol::{SymbolNode, SymbolNodeRef},
        SymbolTable, SymbolTableRef,
    },
};

#[allow(dead_code)]
pub struct SemanticAnalyzer {
    symbol_table: SymbolTableRef,
    program: ProgramP,
}

pub trait AstAnalyze: Sized {
    fn analyze(&mut self, symbol_table: &mut SymbolTable) -> Result<TypeSpecifier>;
}

impl SemanticAnalyzer {
    pub fn new(symbol_table: SymbolTableRef, program: ProgramP) -> Self {
        Self {
            symbol_table,
            program,
        }
    }

    pub fn analyze(&mut self) -> Result<()> {
        self.init_symbol_table()?;
        // let mut symbol_table = self.symbol_table.borrow_mut();
        // let mut program = self.program.borrow_mut();
        // program.analyze(&mut symbol_table)?;
        return Ok(());
    }

    fn init_symbol_table(&mut self) -> Result<()> {
        let table = self.symbol_table.borrow();
        let root = table.root();
        for decl in self.program.borrow().declarations.iter() {
            let node: SymbolNodeRef = match decl {
                Declarations::Fn(fn_decl) => SymbolNode::from((fn_decl, root.clone())),
                Declarations::Struct(struct_decl) => SymbolNode::from((struct_decl, root.clone())),
                Declarations::Enum(enum_decl) => SymbolNode::from((enum_decl, root.clone())),
                Declarations::Impl(impl_decl) => {
                    SymbolNode::append_impl(impl_decl, root.clone())?;
                    continue;
                }
            }
            .into();
            table.root().borrow_mut().append(node);
        }
        return Ok(());
    }
}
