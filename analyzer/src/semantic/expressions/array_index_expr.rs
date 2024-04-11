use crate::{
    ast::{
        expressions::{
            array_index_expr::ArrayIndexExpression,
            unary_expr::{UnaryExpressionType, UnaryOperators},
            Expression,
        },
        type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for ArrayIndexExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        Self::analyze_index(self.index.as_mut(), parent.clone(), root.clone())?;

        if let Some(index) = self.second_index.as_mut() {
            Self::analyze_index(index.as_mut(), parent.clone(), root.clone())?;
        }

        let result = matches!(
            self.array.as_ref(),
            Expression::Identifier(_) | Expression::MemberAccess(_) | Expression::FnCall(_)
        );

        if !result {
            return Err(CompilerError::Semantic(String::from(
                "Array type must be Identifier, Member Access or Function Call expression",
            )));
        }
        let expr_type = self.array.analyze(parent, root)?;

        return match expr_type {
            TypeSpecifier::ArrayType { type_specifier, .. } => {
                if type_specifier.as_ref() == &TypeSpecifier::Str {
                    Ok(TypeSpecifier::Char)
                } else if let TypeSpecifier::ArrayType { type_specifier, .. } =
                    type_specifier.as_ref()
                {
                    Ok(type_specifier.as_ref().clone())
                } else {
                    Ok(type_specifier.as_ref().clone())
                }
            }
            TypeSpecifier::Pointer(type_specifier) => Ok(type_specifier.as_ref().clone()),
            _ => Err(CompilerError::Semantic(String::from(
                "Expect the array type",
            ))),
        };
    }
}

impl ArrayIndexExpression {
    fn analyze_index(
        index: &mut Expression,
        parent: SymbolNodeRef,
        root: SymbolNodeRef,
    ) -> Result<()> {
        let result = matches!(index, Expression::ArrayInit(_) | Expression::ObjectInit(_));
        if result {
            return Err(CompilerError::Semantic(String::from(
                "Index cannot be an object or array init expression",
            )));
        }

        if let Expression::Unary(unary_expr) = index {
            if let UnaryExpressionType::Prefix = unary_expr.expression_type {
                if let UnaryOperators::Minus = unary_expr.operator {
                    return Err(CompilerError::Semantic(String::from(
                        "Index cannot be negative",
                    )));
                }
            }
        }

        let type_specifier = index.analyze(parent, root)?;

        if let TypeSpecifier::Usize | TypeSpecifier::Int = type_specifier {
            return Ok(());
        }

        return Err(CompilerError::Semantic(String::from(
            "Array index type must be usize",
        )));
    }
}
