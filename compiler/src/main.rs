use std::fs;

use analyzer::{error::CompilerError, lexer::Lexer, parser::Parser};

fn run() -> Result<(), CompilerError> {
    let file_name = String::from("main.mb");
    let content = fs::read_to_string(&file_name).expect("File not found");
    let lexer = Lexer::new(content, file_name);
    let mut parser = Parser::new(lexer)?;
    let program = parser.parse()?;

    println!("{:#?}", program);
    return Ok(());
}

fn main() {
    if let Err(err) = run() {
        println!("{err}");
    }
}
