use std::fs;

use analyzer::{
    ast::program::ProgramRef,
    error::Result,
    lexer::Lexer,
    parser::Parser,
    semantic::SemanticAnalyzer,
    symbol_table::{SymbolTable, SymbolTableRef},
};

pub struct Compiler {
    file_name: String,
    symbol_table: SymbolTableRef,
}

impl Compiler {
    pub fn new(file_name: String) -> Self {
        return Compiler {
            file_name,
            symbol_table: SymbolTable::new().into(),
        };
    }

    pub fn analyze(&self) -> Result<()> {
        let program: ProgramRef = self.parse_program()?;
        self.analyze_program(program.clone())?;

        println!("{:#?}", program);
        println!("{}", self.symbol_table.borrow());
        return Ok(());
    }

    fn parse_program(&self) -> Result<ProgramRef> {
        let content = self.read_file();
        let lexer = Lexer::new(content, self.file_name.clone(), self.symbol_table.clone());
        let mut parser = Parser::new(lexer, self.symbol_table.clone())?;
        return Ok(parser.parse()?.into());
    }

    fn analyze_program(&self, program: ProgramRef) -> Result<()> {
        let mut semantic_analyzer = SemanticAnalyzer::new(self.symbol_table.clone(), program);
        return semantic_analyzer.analyze();
    }

    fn read_file(&self) -> String {
        return fs::read_to_string(&self.file_name).expect("File not found");
    }
}
