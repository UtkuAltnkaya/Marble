use std::error::Error;

use super::Parser;

#[derive(Debug)]
pub struct SyntacticError {
    pub content: String,
}

impl std::fmt::Display for SyntacticError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.content)
    }
}

impl Error for SyntacticError {}

#[allow(dead_code)]
impl SyntacticError {
    pub fn new(parser: &Parser, err: String) -> Self {
        Self {
            content: Self::handle_content(parser, err),
        }
    }

    fn handle_content(parser: &Parser, err: String) -> String {
        let mut content = format!(
            "{}:{}:{}\n\n",
            parser.lexer.file_name(),
            parser.current.span().start().row(),
            parser.current.span().start().col(),
        );

        if let Some(_) = (parser.current.span().start().row()).checked_sub(3) {
            content += format!("{:-2} |\n", parser.current.span().start().row() - 2).as_str();
            content += format!("{:-2} |\n", parser.current.span().start().row() - 1).as_str();
        }
        let byte = Self::calculate_println(&mut content, parser);
        Self::print_content(&mut content, parser);
        Self::print_point(&mut content, parser, byte);
        content += format!("   |\t{}\n", err).as_str();
        content += format!("{:-2} |\n", parser.current.span().start().row() + 1).as_str();

        return content;
    }

    fn calculate_println(content: &mut String, parser: &Parser) -> usize {
        content.push_str("   |\t");
        let item = format!(
            "{}-{}:  ",
            parser.current.span().start().row(),
            parser.current.span().start().col(),
        );
        content.push_str(item.as_str());
        return item.len();
    }

    fn print_content(content: &mut String, parser: &Parser) {
        let mut i = Self::init_content_start(parser) + 1;
        let mut str = String::new();
        while i != parser.lexer.content().len()
            // && parser.lexer.content()[i] != b';'
            && parser.lexer.content()[i + 1] != b'\n'
            && parser.lexer.content()[i] != 0
        {
            str.push(char::from(parser.lexer.content()[i]));
            i += 1;
        }
        // str.push(char::from(parser.lexer.content()[i]));
        content.push_str(format!("{}\n", str).as_str());
    }

    fn print_point(content: &mut String, parser: &Parser, byte: usize) {
        let mut item = String::from("   |\t");
        let space = std::iter::repeat(' ')
            .take(parser.current.span().start().col() + byte - 1)
            .collect::<String>();

        item += space.as_str();
        for _ in 0..(parser.current.text().len()) {
            item.push('^');
        }
        content.push_str(format!("{}\n", item).as_str());
    }

    fn init_content_start(parser: &Parser) -> usize {
        let mut i = 0;
        parser
            .lexer
            .content()
            .iter()
            .take_while(|ch| {
                if **ch == b'\n' {
                    i += 1;
                }
                i < parser.current.span().start().row() - 1
            })
            .count()
    }
}
