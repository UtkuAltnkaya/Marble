pub mod keywords;
pub mod lexical_error;
pub mod token;
pub mod token_finder;
pub mod token_type;

use std::collections::HashMap;

use crate::{error::CompilerError, error_lexer};

use self::{keywords::init_keywords, token::Token, token_type::TokenType};

#[allow(dead_code)]
pub struct Lexer {
    content: Vec<u8>,
    cursor: usize,
    cursor_start: usize,
    line: usize,
    line_start: usize,
    file_name: String,
    keywords: HashMap<String, TokenType>,
}

impl Lexer {
    pub fn new(content: String, file_name: String) -> Self {
        Self {
            content: content.into_bytes(),
            file_name,
            cursor: 0,
            cursor_start: 0,
            line_start: 0,
            line: 0,
            keywords: init_keywords(),
        }
    }

    pub fn next_token(&mut self) -> Result<Token, CompilerError> {
        self.skip_space();

        match self.current_char() {
            0 => Ok(Token::get_eof(self)),
            b'a'..=b'z' | b'A'..=b'Z' | b'_' => Ok(Token::get_id_or_keyword(self)),
            b'0'..=b'9' => Token::get_number(self),
            b'"' => Token::get_string(self),
            b'\'' => Token::get_char(self),
            _ => {
                if let Some(token) = Token::get_operator(self) {
                    Ok(token)
                } else if let Some(token) = Token::get_delimiters(self) {
                    Ok(token)
                } else {
                    return error_lexer!(&self, "Unknown token!");
                }
            }
        }
    }

    fn skip_space(&mut self) {
        while self.current_char().is_ascii_whitespace() {
            if self.current_char() == b'\n' {
                self.line_start = self.cursor;
                self.line += 1;
            }
            self.next_char();
        }
    }
    pub(crate) fn current_char(&self) -> u8 {
        return if self.cursor < self.content.len() {
            self.content[self.cursor]
        } else {
            0
        };
    }

    pub fn file_name(&self) -> &str {
        self.file_name.as_ref()
    }

    pub(crate) fn next_char(&mut self) {
        self.cursor += 1;
    }

    pub(crate) fn set_cursor_start(&mut self) {
        self.cursor_start = self.cursor;
    }

    /*
    pub(crate) fn line(&self) -> usize {
        self.line
    }

    pub(crate) fn cursor(&self) -> usize {
        self.cursor
    }

    pub(crate) fn cursor_start(&self) -> usize {
        self.cursor_start
    }

    pub(crate) fn line_start(&self) -> usize {
        self.line_start
    }
    */

    pub(crate) fn content(&self) -> &[u8] {
        self.content.as_ref()
    }
}
