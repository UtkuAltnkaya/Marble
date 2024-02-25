use std::{error::Error, fmt};

use crate::{lexer::lexical_error::LexicalError, parser::syntactic_error::SyntacticError};

pub type Result<T> = std::result::Result<T, CompilerError>;

#[derive(Debug)]
pub enum CompilerError {
    Lexical(LexicalError),
    Syntactic(SyntacticError),
    Semantic(String),
}

impl Error for CompilerError {
    fn source(&self) -> Option<&(dyn Error + 'static)> {
        match self {
            CompilerError::Lexical(lexical) => Some(lexical),
            CompilerError::Syntactic(syntactic) => Some(syntactic),
            CompilerError::Semantic(_semantic) => None,
        }
    }

    fn cause(&self) -> Option<&dyn Error> {
        self.source()
    }
}

impl fmt::Display for CompilerError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            CompilerError::Lexical(lexical) => write!(f, "{}", lexical),
            CompilerError::Syntactic(syntactic) => write!(f, "{}", syntactic),
            CompilerError::Semantic(semantic) => write!(f, "{}", semantic),
        }
    }
}

#[macro_export]
macro_rules! error_lexer {
    ($lexer:expr,$msg:expr) => {
        Err(crate::error::CompilerError::Lexical(
            crate::lexer::lexical_error::LexicalError::new($lexer, String::from($msg)),
        ))
    };
}

#[macro_export]
macro_rules! error_parser {
    ($parser:expr,$msg:expr) => {
        Err(crate::error::CompilerError::Syntactic(
            crate::parser::syntactic_error::SyntacticError::new($parser, String::from($msg)),
        ))
    };
}
