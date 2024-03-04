pub mod array_index_expr;
pub mod array_init_expr;
pub mod assignment_expr;
pub mod binary_expr;
pub mod cast_expr;
pub mod fn_call_expr;
pub mod identifier_expression;
pub mod member_access_expr;
pub mod object_init_expr;
pub mod primitive_expr;
pub mod unary_expr;

use crate::{error::Result, parser::Parser};

use self::{
    array_index_expr::ArrayIndexExpression,
    array_init_expr::ArrayInitExpression,
    assignment_expr::AssignmentExpression,
    binary_expr::{BinaryExpression, BinaryPrecedence},
    cast_expr::CastExpression,
    fn_call_expr::FnCallExpression,
    identifier_expression::IdentifierExpression,
    member_access_expr::MemberAccessExpression,
    object_init_expr::ObjectInitExpression,
    primitive_expr::PrimitiveExpression,
    unary_expr::UnaryExpression,
};

use super::AstParse;

#[derive(Debug)]
pub enum Expression {
    Binary(BinaryExpression),
    Assignment(AssignmentExpression),
    Unary(UnaryExpression),
    ArrayIndex(ArrayIndexExpression),
    MemberAccess(MemberAccessExpression),
    FnCall(FnCallExpression),
    ArrayInit(ArrayInitExpression),
    ObjectInit(ObjectInitExpression),
    Identifier(IdentifierExpression),
    Primitive(PrimitiveExpression),
    Cast(CastExpression),
}

#[derive(Clone)]
pub enum Precedence {
    Binary,
    Assignment,
    Unary,
    ArrayIndex,
    MemberAccess,
    FunctionCall,
    ArrayInit,
    ObjectInit,
    Cast,
    Identifier,
    Primitive,
}

impl AstParse for Expression {
    fn parse(parser: &mut Parser) -> Result<Self> {
        return Self::parse_expr(parser, Precedence::default());
    }
}

impl Expression {
    pub(super) fn parse_expr(parser: &mut Parser, precedence: Precedence) -> Result<Self> {
        match precedence {
            Precedence::Binary => {
                BinaryExpression::parse(parser, &precedence, BinaryPrecedence::default())
            }
            Precedence::Assignment => AssignmentExpression::parse(parser, precedence),
            Precedence::Unary => UnaryExpression::parse(parser, precedence),
            Precedence::ArrayIndex => ArrayIndexExpression::parse(parser, precedence),
            Precedence::MemberAccess => MemberAccessExpression::parse(parser, precedence),
            Precedence::FunctionCall => FnCallExpression::parse(parser, precedence),
            Precedence::ArrayInit => ArrayInitExpression::parse(parser, precedence),
            Precedence::ObjectInit => ObjectInitExpression::parse(parser, precedence),
            Precedence::Cast => CastExpression::parse(parser, precedence),
            Precedence::Identifier => IdentifierExpression::parse(parser, precedence),
            Precedence::Primitive => PrimitiveExpression::parse(parser),
        }
    }
}

impl Precedence {
    #[inline(always)]
    pub fn next(&self) -> Precedence {
        match *self {
            Precedence::Binary => Precedence::Assignment,
            Precedence::Assignment => Precedence::Unary,
            Precedence::Unary => Precedence::ArrayIndex,
            Precedence::ArrayIndex => Precedence::MemberAccess,
            Precedence::MemberAccess => Precedence::FunctionCall,
            Precedence::FunctionCall => Precedence::ArrayInit,
            Precedence::ArrayInit => Precedence::ObjectInit,
            Precedence::ObjectInit => Precedence::Cast,
            Precedence::Cast => Precedence::Identifier,
            Precedence::Identifier => Precedence::Primitive,
            Precedence::Primitive => Precedence::Primitive,
        }
    }
}

impl Default for Precedence {
    fn default() -> Self {
        return Self::Binary;
    }
}
