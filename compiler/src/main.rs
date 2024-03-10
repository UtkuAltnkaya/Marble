use std::{cell::RefCell, fs, rc::Rc};

use analyzer::{
    ast::program::ProgramRef,
    error::Result,
    lexer::Lexer,
    parser::Parser,
    semantic::SemanticAnalyzer,
    symbol_table::{SymbolTable, SymbolTableRef},
};

fn run() -> Result<()> {
    let file_name = String::from("main.mb");
    let content = fs::read_to_string(&file_name).expect("File not found");

    let symbol_table: SymbolTableRef = Rc::new(RefCell::new(SymbolTable::new()));
    let lexer = Lexer::new(content, file_name, Rc::clone(&symbol_table));
    let mut parser = Parser::new(lexer, Rc::clone(&symbol_table))?;

    let program: ProgramRef = Rc::new(RefCell::new(parser.parse()?));
    let mut semantic_analyzer = SemanticAnalyzer::new(symbol_table.clone(), program.clone());

    semantic_analyzer.analyze()?;

    println!("Program: {:#?}\n\n", program.borrow());
    println!("SymbolTable: {{\n{}}}", symbol_table.borrow());
    return Ok(());
}

fn main() {
    if let Err(err) = run() {
        println!("{err}");
    }
}
