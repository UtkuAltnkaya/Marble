pub mod array_index_expr;
pub mod array_init_expr;
pub mod binary_expr;
pub mod cast_expr;
pub mod fn_call_expr;
pub mod identifier_expr;
pub mod member_access_expr;
pub mod object_init_expr;
pub mod primitive_expr;

use crate::{
    ast::{
        expressions::{identifier_expression::IdentifierExpression, Expression},
        type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    symbol_table::symbol::SymbolNodeRef,
};

use super::AstAnalyze;

impl AstAnalyze for Expression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        return match self {
            Expression::Binary(binary_expr) => binary_expr.analyze(parent, root),
            Expression::Assignment(_) => todo!(),
            Expression::Unary(_) => todo!(),
            Expression::ArrayIndex(array_index) => array_index.analyze(parent, root),
            Expression::MemberAccess(member_access) => member_access.analyze(parent, root),
            Expression::FnCall(fn_call_expr) => fn_call_expr.analyze(parent, root),
            Expression::ArrayInit(array_init_expr) => array_init_expr.analyze(parent, root),
            Expression::ObjectInit(object_init_expr) => object_init_expr.analyze(parent, root),
            Expression::Cast(cast_expr) => cast_expr.analyze(parent, root),
            Expression::Identifier(identifier_expr) => identifier_expr.analyze(parent, root),
            Expression::Primitive(primitive_expr) => primitive_expr.analyze(parent, root),
        };
    }
}

impl Expression {
    fn identifier(&self) -> Result<&IdentifierExpression> {
        if let Expression::Identifier(identifier) = self {
            return Ok(identifier);
        }
        return Err(CompilerError::Semantic(String::from(
            "Expected Identifier Expression",
        )));
    }
}
