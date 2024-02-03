pub mod ast;
pub mod error;
pub mod lexer;
pub mod parser;
pub mod semantic;
pub mod symbol_table;

#[macro_export]
macro_rules! debug_current_token {
    ($parser:expr) => {
        dbg!($parser.current());
    };
}

#[macro_export]
macro_rules! debug_next_token {
    ($parser:expr) => {
        dbg!($parser.next());
    };
}
