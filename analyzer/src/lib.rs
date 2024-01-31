pub mod ast;
pub mod error;
pub mod lexer;
pub mod parser;

#[macro_export]
macro_rules! debug_current_token {
    ($parser:expr) => {
        println!("{:#?}", $parser.current());
    };
}

#[macro_export]
macro_rules! debug_next_token {
    ($parser:expr) => {
        println!("{:#?}", $parser.next());
    };
}
