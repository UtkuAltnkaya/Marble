use super::token_type::TokenType;

#[derive(Debug, Clone, PartialEq)]
pub struct Token {
    token_type: TokenType,
    span: Span,
    text: String,
}

#[derive(Debug, Clone, PartialEq)]
pub struct Span {
    start: Position,
    end: Position,
}

#[derive(Debug, Clone, PartialEq)]
pub struct Position {
    row: usize,
    col: usize,
    cursor_start: usize,
}

impl Token {
    pub fn new(token_type: TokenType, span: Span, text: String) -> Self {
        Self {
            token_type,
            span,
            text,
        }
    }

    pub fn token_type(&self) -> &TokenType {
        &self.token_type
    }

    pub fn span(&self) -> &Span {
        &self.span
    }

    pub fn text(&self) -> &str {
        self.text.as_ref()
    }
}

impl Span {
    pub fn new(start: Position, end: Position) -> Self {
        Self { start, end }
    }

    pub fn start(&self) -> &Position {
        &self.start
    }

    pub fn end(&self) -> &Position {
        &self.end
    }
}

impl Position {
    pub fn new(row: usize, col: usize, cursor_start: usize) -> Self {
        Self {
            row,
            col,
            cursor_start,
        }
    }

    pub fn row(&self) -> usize {
        self.row
    }

    pub fn col(&self) -> usize {
        self.col
    }

    pub fn cursor_start(&self) -> usize {
        self.cursor_start
    }
}
