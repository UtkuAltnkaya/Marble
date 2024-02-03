use crate::{
    ast::{expressions::Expression, AstParse},
    error::Result,
    lexer::token_type::TokenType,
    parser::Parser,
};

use super::block_stmt::BlockStmt;

#[derive(Debug)]
pub struct IfStmt {
    pub condition: Expression,
    pub block: BlockStmt,
    pub else_ifs: Vec<ElseIfStmt>,
    pub else_stmt: Option<BlockStmt>,
}

#[derive(Debug)]
pub struct ElseIfStmt {
    pub condition: Expression,
    pub block: BlockStmt,
}

impl AstParse for IfStmt {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let (condition, block) = Self::parse_condition(parser)?;
        let else_ifs = Self::parse_else_if(parser)?;
        let else_stmt = Self::parse_else(parser)?;
        return Ok(IfStmt::new(condition, block, else_ifs, else_stmt));
    }
}

impl IfStmt {
    pub fn new(
        condition: Expression,
        block: BlockStmt,
        else_ifs: Vec<ElseIfStmt>,
        else_stmt: Option<BlockStmt>,
    ) -> Self {
        Self {
            condition,
            block,
            else_ifs,
            else_stmt,
        }
    }

    fn parse_condition(parser: &mut Parser) -> Result<(Expression, BlockStmt)> {
        parser.next_token_and_expect(TokenType::OpenParen)?;
        parser.next_token()?;
        let condition = Expression::parse(parser)?;
        parser.next_token_and_expect(TokenType::CloseParen)?;
        let block = BlockStmt::parse(parser)?;
        return Ok((condition, block));
    }

    fn parse_else_if(parser: &mut Parser) -> Result<Vec<ElseIfStmt>> {
        let mut vec = Vec::new();
        ElseIfStmt::parse_else_if(parser, &mut vec)?;
        return Ok(vec);
    }

    fn parse_else(parser: &mut Parser) -> Result<Option<BlockStmt>> {
        if parser.current().token_type() != &TokenType::Else {
            return Ok(None);
        }

        return Ok(Some(BlockStmt::parse(parser)?));
    }
}

impl AstParse for ElseIfStmt {
    fn parse(parser: &mut Parser) -> Result<Self> {
        let (condition, block) = IfStmt::parse_condition(parser)?;
        return Ok(Self::new(condition, block));
    }
}

impl ElseIfStmt {
    pub fn new(condition: Expression, block: BlockStmt) -> Self {
        Self { condition, block }
    }

    fn parse_else_if(parser: &mut Parser, vec: &mut Vec<Self>) -> Result<()> {
        if parser.next().token_type() != &TokenType::Else {
            return Ok(());
        }
        parser.next_token()?;
        if parser.next().token_type() != &TokenType::If {
            return Ok(());
        }
        parser.next_token()?;
        vec.push(Self::parse(parser)?);
        return Self::parse_else_if(parser, vec);
    }
}
