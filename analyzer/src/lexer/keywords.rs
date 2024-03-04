use super::token_type::TokenType;
use std::collections::HashMap;

pub fn init_keywords() -> HashMap<String, TokenType> {
    let mut map = HashMap::new();
    map.insert(String::from("let"), TokenType::Let);
    map.insert(String::from("fn"), TokenType::Fn);
    map.insert(String::from("break"), TokenType::Break);
    map.insert(String::from("case"), TokenType::Case);
    map.insert(String::from("char"), TokenType::CharKeyword);
    map.insert(String::from("const"), TokenType::Const);
    map.insert(String::from("continue"), TokenType::Continue);
    map.insert(String::from("default"), TokenType::Default);
    map.insert(String::from("do"), TokenType::Do);
    map.insert(String::from("double"), TokenType::Double);
    map.insert(String::from("else"), TokenType::Else);
    map.insert(String::from("enum"), TokenType::Enum);
    map.insert(String::from("float"), TokenType::Float);
    map.insert(String::from("for"), TokenType::For);
    map.insert(String::from("if"), TokenType::If);
    map.insert(String::from("int"), TokenType::Int);
    map.insert(String::from("bool"), TokenType::Bool);
    map.insert(String::from("return"), TokenType::Return);
    map.insert(String::from("usize"), TokenType::Usize);
    map.insert(String::from("sizeof"), TokenType::Sizeof);
    map.insert(String::from("static"), TokenType::Static);
    map.insert(String::from("struct"), TokenType::Struct);
    map.insert(String::from("switch"), TokenType::Switch);
    map.insert(String::from("void"), TokenType::Void);
    map.insert(String::from("while"), TokenType::While);
    map.insert(String::from("str"), TokenType::Str);
    map.insert(String::from("impl"), TokenType::Impl);
    map.insert(String::from("pub"), TokenType::Pub);
    map.insert(String::from("true"), TokenType::True);
    map.insert(String::from("false"), TokenType::False);
    map.insert(String::from("defer"), TokenType::Defer);
    map.insert(String::from("as"), TokenType::As);
    return map;
}
