use crate::{error::Result, error_parser, lexer::token_type::TokenType, parser::Parser};

pub struct Parenthesis;

impl Parenthesis {
    pub fn parse<T, F>(parser: &mut Parser, close: TokenType, callback: F) -> Result<Vec<T>>
    where
        F: Fn(&mut Parser) -> Result<T>,
    {
        let mut vec = Vec::new();
        while parser.current().token_type() != &close {
            parser.next_token()?;
            if parser.current().token_type() == &close {
                break;
            }
            if parser.current().token_type() == &TokenType::Eof {
                return error_parser!(parser, format!("Missing {}", close));
            }
            vec.push(callback(parser)?);
            parser.next_token()?;
            if parser.current().token_type() == &TokenType::Comma {
                continue;
            }
            if parser.current().token_type() == &close {
                break;
            }
            return error_parser!(
                parser,
                format!(
                    "Expect Comma or {} but found {}",
                    close,
                    parser.current().token_type(),
                )
            );
        }

        return Ok(vec);
    }
}
