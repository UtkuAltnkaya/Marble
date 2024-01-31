use crate::lexer::Lexer;
use std::{error::Error, fmt};

#[derive(Debug)]
pub struct LexicalError {
    content: String,
}

impl fmt::Display for LexicalError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.content)
    }
}

impl Error for LexicalError {}

impl LexicalError {
    pub fn new(lexer: &Lexer, msg: String) -> Self {
        Self {
            content: Self::handle_content(lexer, msg),
        }
    }

    fn handle_content(lexer: &Lexer, msg: String) -> String {
        let mut content = format!(
            "{}:{}:{}\n\n",
            lexer.file_name,
            lexer.line + 1,
            lexer.cursor - lexer.line_start
        );

        if let Some(_) = (lexer.line).checked_sub(1) {
            content += format!("{:-2} |\n", lexer.line - 1).as_str();
            content += format!("{:-2} |\n", lexer.line).as_str();
        }
        let byte = Self::calculate_println(&mut content, lexer);
        Self::print_content(&mut content, lexer);
        Self::print_point(&mut content, lexer, byte);
        content += format!("   |\t{}\n", msg).as_str();
        content += format!("{:-2} |\n", lexer.line + 2).as_str();
        return content;
    }

    fn calculate_println(content: &mut String, lexer: &Lexer) -> usize {
        content.push_str("   |\t");
        let item = format!("{}-{}:  ", lexer.line + 1, lexer.cursor - lexer.line_start);
        content.push_str(item.as_str());
        return item.len();
    }

    fn print_content(content: &mut String, lexer: &Lexer) {
        let mut i = if lexer.content[lexer.line_start] == b'\n' {
            lexer.line_start + 1
        } else {
            lexer.line_start
        };

        let mut str = String::new();
        while i != lexer.content.len()
            && lexer.content[i] != b';'
            && lexer.content[i + 1] != b'\n'
            && lexer.content[i] != 0
        {
            str.push(char::from(lexer.content[i]));
            i += 1;
        }
        str.push(char::from(lexer.content[i]));
        content.push_str(format!("{}\n", str).as_str());
    }

    fn print_point(content: &mut String, lexer: &Lexer, byte: usize) {
        let mut item = String::from("   |\t");
        let space = std::iter::repeat(' ')
            .take(lexer.cursor_start - lexer.line_start + byte - 1)
            .collect::<String>();

        item += space.as_str();
        if lexer.cursor == lexer.cursor_start {
            item.push('^');
        } else {
            for _ in 0..(lexer.cursor - lexer.cursor_start) {
                item.push('^');
            }
        }
        content.push_str(format!("{}\n", item).as_str());
    }
}
