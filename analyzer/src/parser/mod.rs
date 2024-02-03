pub mod syntactic_error;

use crate::{
    ast::{program::Program, AstParse},
    error::Result,
    error_parser,
    lexer::{token::Token, token_type::TokenType, Lexer},
    symbol_table::SymbolTableP,
};

#[allow(dead_code)]
pub struct Parser {
    lexer: Lexer,
    previous: Option<Token>,
    current: Token,
    next: Token,
    symbol_table: SymbolTableP,
}

#[allow(dead_code)]
impl Parser {
    pub fn new(mut lexer: Lexer, symbol_table: SymbolTableP) -> Result<Self> {
        Ok(Self {
            previous: None,
            current: lexer.next_token()?,
            next: lexer.next_token()?,
            lexer,
            symbol_table,
        })
    }

    pub fn parse(&mut self) -> Result<Program> {
        return Program::parse(self);
    }

    pub(crate) fn next_token(&mut self) -> Result<()> {
        self.previous = Some(std::mem::replace(
            &mut self.current,
            std::mem::replace(&mut self.next, self.lexer.next_token()?),
        ));
        return Ok(());
    }

    pub(crate) fn expect(&mut self, token_type: TokenType) -> Result<&Token> {
        if self.current.token_type() != &token_type {
            return error_parser!(
                self,
                &format!("Expect {token_type} found {}", self.current.token_type())
            );
        }
        return Ok(&self.current);
    }

    pub(crate) fn next_token_and_expect(&mut self, token_type: TokenType) -> Result<&Token> {
        self.next_token()?;
        return self.expect(token_type);
    }

    pub fn previous(&self) -> Option<&Token> {
        self.previous.as_ref()
    }

    pub fn current(&self) -> &Token {
        &self.current
    }

    pub fn next(&self) -> &Token {
        &self.next
    }
}
