use crate::{
    ast::{
        access_specifier::AccessSpecifier, identifier::Identifier, parenthesis::Parenthesis,
        variable_type::VariableType, AstParse,
    },
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
    symbol_table::ToSymbol,
};

#[derive(Debug)]
pub struct StructDeclaration {
    pub access_specifier: AccessSpecifier,
    pub name: Identifier,
    pub fields: Vec<StructField>,
}

#[derive(Debug)]
pub struct StructField {
    pub access_specifier: AccessSpecifier,
    pub field: VariableType,
}

impl AstParse for StructDeclaration {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let name = Identifier::parse(parser)?;
        parser.next_token_and_expect(TokenType::OpenCurlyBrace)?;
        let fields = Parenthesis::parse(parser, TokenType::CloseCurlyBrace, |parser| {
            return StructField::parse(parser);
        })?;
        let struct_decl = Self::new(AccessSpecifier::Private, name, fields);
        struct_decl.to_symbol(parser.symbol_table().borrow().root())?;
        return Ok(struct_decl);
    }
}

impl StructDeclaration {
    pub fn new(
        access_specifier: AccessSpecifier,
        name: Identifier,
        fields: Vec<StructField>,
    ) -> Self {
        Self {
            access_specifier,
            name,
            fields,
        }
    }
}
impl AstParse for StructField {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let access_specifier = AccessSpecifier::parse(parser)?;
        let field = VariableType::parse(parser)?;
        return Ok(Self::new(access_specifier, field));
    }
}

impl StructField {
    pub fn new(access_specifier: AccessSpecifier, field: VariableType) -> Self {
        Self {
            access_specifier,
            field,
        }
    }
}
