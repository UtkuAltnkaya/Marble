use crate::{error::CompilerError, error_lexer};

use super::{
    token::{Position, Span, Token},
    token_type::TokenType,
    Lexer,
};

impl Token {
    fn from_lexer(lexer: &Lexer, token_type: TokenType) -> Self {
        return Self::new(
            token_type,
            Span::new(
                Position::new(
                    lexer.line + 1,
                    lexer.cursor_start - lexer.line_start,
                    lexer.cursor_start,
                ),
                Position::new(
                    lexer.line + 1,
                    lexer.cursor - lexer.line_start,
                    lexer.cursor,
                ),
            ),
            String::from_utf8_lossy(&lexer.content[lexer.cursor_start..lexer.cursor]).to_string(),
        );
    }

    pub(crate) fn get_eof(lexer: &mut Lexer) -> Self {
        return Token::new(
            TokenType::Eof,
            Span::new(
                Position::new(
                    lexer.line + 1,
                    lexer.cursor - lexer.line_start + 1,
                    lexer.cursor_start,
                ),
                Position::new(
                    lexer.line + 1,
                    lexer.cursor - lexer.line_start + 1,
                    lexer.cursor,
                ),
            ),
            String::from("EOF"),
        );
    }

    pub(crate) fn get_id_or_keyword(lexer: &mut Lexer) -> Self {
        lexer.set_cursor_start();

        while lexer.current_char().is_ascii_alphabetic()
            || lexer.current_char() == b'_'
            || lexer.current_char().is_ascii_digit()
        {
            lexer.next_char();
        }
        let text =
            String::from_utf8_lossy(&lexer.content[lexer.cursor_start..lexer.cursor]).to_string();
        let start = Position::new(
            lexer.line + 1,
            lexer.cursor_start - lexer.line_start,
            lexer.cursor_start,
        );
        let end = Position::new(
            lexer.line + 1,
            lexer.cursor - lexer.line_start,
            lexer.cursor,
        );
        match lexer.keywords.get(&text) {
            Some(token_type) => Self::new(token_type.to_owned(), Span::new(start, end), text),
            None => Self::new(TokenType::Identifier, Span::new(start, end), text),
        }
    }

    pub(crate) fn get_number(lexer: &mut Lexer) -> Result<Token, CompilerError> {
        let mut count: u32 = 0;
        lexer.cursor_start = lexer.cursor;
        while lexer.current_char().is_ascii_digit() || lexer.current_char() == b'.' {
            if lexer.current_char() == b'.' {
                count += 1;
            }
            lexer.next_char();
        }
        if count > 1 {
            return error_lexer!(lexer, "'.' occurs more than once!");
        }

        return Ok(Self::from_lexer(&lexer, TokenType::Number));
    }

    pub(crate) fn get_string(lexer: &mut Lexer) -> Result<Self, CompilerError> {
        lexer.next_char();
        lexer.cursor_start = lexer.cursor;

        while lexer.current_char() != b'"' {
            if lexer.cursor >= lexer.content.len() {
                return error_lexer!(lexer, "\" not closed");
            }
            Self::is_escape_char(lexer)?;
            if lexer.current_char() == b'\n' {
                return error_lexer!(lexer, "String should not be contain new line");
            }
            lexer.next_char();
        }

        let token = Self::from_lexer(&lexer, TokenType::String);

        lexer.next_char();
        return Ok(token);
    }

    pub(crate) fn get_char(lexer: &mut Lexer) -> Result<Self, CompilerError> {
        lexer.next_char();
        lexer.cursor_start = lexer.cursor;
        Self::is_escape_char(lexer)?;
        lexer.next_char();
        let token = Self::from_lexer(&lexer, TokenType::Char);
        lexer.next_char();
        return Ok(token);
    }

    pub(crate) fn is_escape_char(lexer: &mut super::Lexer) -> Result<(), CompilerError> {
        if lexer.current_char() == b'\\' {
            lexer.next_char();
            if !Self::check_escape(&lexer.current_char()) {
                return error_lexer!(lexer, "Unknown escape sequence");
            }
        }
        return Ok(());
    }

    pub(crate) fn check_escape(ch: &u8) -> bool {
        match ch {
            b'a' => true,
            b'b' => true,
            b'f' => true,
            b'n' => true,
            b'r' => true,
            b't' => true,
            b'v' => true,
            b'\\' => true,
            b'\'' => true,
            b'"' => true,
            b'0' => true,
            _ => false,
        }
    }

    pub(crate) fn get_delimiters(lexer: &mut Lexer) -> Option<Self> {
        let token_type = match lexer.current_char() {
            b'(' => TokenType::OpenParen,
            b')' => TokenType::CloseParen,
            b'{' => TokenType::OpenCurlyBrace,
            b'}' => TokenType::CloseCurlyBrace,
            b'[' => TokenType::OpenBracket,
            b']' => TokenType::CloseBracket,
            b',' => TokenType::Comma,
            b'.' => TokenType::Dot,
            b';' => TokenType::Semicolon,
            b':' => TokenType::Colon,
            _ => {
                return None;
            }
        };
        lexer.next_char();
        return Some(Self::from_lexer(lexer, token_type));
    }

    pub(crate) fn get_operator(lexer: &mut Lexer) -> Option<Self> {
        lexer.cursor_start = lexer.cursor;

        let type_specifier = match lexer.current_char() {
            b'+' => {
                if Self::check_next_op(lexer, b'+') {
                    TokenType::Increment
                } else {
                    TokenType::Plus
                }
            }
            b'-' => {
                if Self::check_next_op(lexer, b'>') {
                    TokenType::Arrow
                } else if Self::check_next_op(lexer, b'-') {
                    TokenType::Decrement
                } else {
                    TokenType::Minus
                }
            }
            b'*' => TokenType::Multiply,
            b'/' => TokenType::Divide,
            b'%' => TokenType::Percent,
            b'=' => {
                if Self::check_next_op(lexer, b'=') {
                    TokenType::Equal
                } else {
                    TokenType::Assign
                }
            }
            b'!' => {
                if Self::check_next_op(lexer, b'=') {
                    TokenType::NotEqual
                } else {
                    TokenType::Bang
                }
            }
            b'<' => {
                if Self::check_next_op(lexer, b'=') {
                    TokenType::LessEqual
                } else if Self::check_next_op(lexer, b'<') {
                    TokenType::BitLeft
                } else {
                    TokenType::LessThan
                }
            }
            b'>' => {
                if Self::check_next_op(lexer, b'=') {
                    TokenType::GreaterEqual
                } else if Self::check_next_op(lexer, b'>') {
                    TokenType::BitRight
                } else {
                    TokenType::GreaterThan
                }
            }
            b'&' => {
                if Self::check_next_op(lexer, b'&') {
                    TokenType::And
                } else {
                    TokenType::BitAnd
                }
            }
            b'|' => {
                if Self::check_next_op(lexer, b'|') {
                    TokenType::Or
                } else {
                    TokenType::BitOr
                }
            }
            b'^' => TokenType::BitXor,
            b'~' => TokenType::Tilde,
            _ => return None,
        };

        lexer.next_char();
        return Some(Self::from_lexer(lexer, type_specifier));
    }

    fn check_next_op(lexer: &mut Lexer, txt: u8) -> bool {
        if lexer.cursor > lexer.content.len() {
            return false;
        }
        if lexer.content[lexer.cursor + 1] != txt {
            return false;
        }
        lexer.next_char();
        return true;
    }
}
