use std::{cell::RefCell, fs, rc::Rc};

use analyzer::{
    error::CompilerError, lexer::Lexer, parser::Parser, semantic::SemanticAnalyzer,
    symbol_table::SymbolTable,
};

fn run() -> Result<(), CompilerError> {
    let file_name = String::from("main.mb");
    let content = fs::read_to_string(&file_name).expect("File not found");
    let symbol_table = Rc::new(RefCell::new(SymbolTable::new()));

    let lexer = Lexer::new(content, file_name, Rc::clone(&symbol_table));
    let mut parser = Parser::new(lexer, Rc::clone(&symbol_table))?;

    let program = Rc::new(RefCell::new(parser.parse()?));

    let mut semantic_analyzer =
        SemanticAnalyzer::new(Rc::clone(&symbol_table), Rc::clone(&program));
    semantic_analyzer.analyze()?;

    dbg!(program);
    println!("{}", symbol_table.borrow());
    return Ok(());
}

fn main() {
    if let Err(err) = run() {
        println!("{err}");
    }
}
