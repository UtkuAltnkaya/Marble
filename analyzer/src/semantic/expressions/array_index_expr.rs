use crate::{
    ast::{
        expressions::{array_index_expr::ArrayIndexExpression, Expression},
        type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for ArrayIndexExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        self.analyze_index(parent.clone(), root.clone())?;

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
    fn analyze_index(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<()> {
        let result = matches!(
            self.index.as_ref(),
            Expression::ArrayInit(_) | Expression::ObjectInit(_)
        );
        if result {
            return Err(CompilerError::Semantic(String::from(
                "Index cannot be an object or array init expression",
            )));
        }

        let type_specifier = self.index.analyze(parent, root)?;

        if let TypeSpecifier::Usize = type_specifier {
            return Ok(());
        }

        return Err(CompilerError::Semantic(String::from(
            "Array index type must be usize",
        )));
    }
}
