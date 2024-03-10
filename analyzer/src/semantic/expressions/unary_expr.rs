use crate::{
    ast::{
        expressions::{
            unary_expr::{UnaryExpression, UnaryExpressionType, UnaryOperators},
            Expression,
        },
        type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::SymbolNodeRef,
};

impl AstAnalyze for UnaryExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        match self.expression_type {
            UnaryExpressionType::Prefix => self.analyze_prefix(parent, root),
            UnaryExpressionType::Postfix => self.analyze_postfix(parent, root),
        }
    }
}

impl UnaryExpression {
    fn analyze_prefix(
        &mut self,
        parent: SymbolNodeRef,
        root: SymbolNodeRef,
    ) -> Result<TypeSpecifier> {
        match self.operator {
            UnaryOperators::Plus | UnaryOperators::Minus => self.analyze_arithmetic(parent, root),
            UnaryOperators::Increment | UnaryOperators::Decrement => {
                self.analyze_postfix(parent, root)
            }
            UnaryOperators::LogicalNot | UnaryOperators::BitwiseNot => {
                self.analyze_not(parent, root)
            }
            UnaryOperators::Address => self.analyze_address(parent, root),
            UnaryOperators::Indirection => self.analyze_pointer(parent, root),
        }
    }

    fn analyze_postfix(
        &mut self,
        parent: SymbolNodeRef,
        root: SymbolNodeRef,
    ) -> Result<TypeSpecifier> {
        let result = matches!(
            self.value.as_ref(),
            Expression::Identifier(_)
                | Expression::MemberAccess(_)
                | Expression::ArrayIndex(_)
                | Expression::Unary(_)
        );
        if !result {
            return Err(CompilerError::Semantic(String::from(
                "Unexpected expression",
            )));
        }
        let expr_type = self.value.analyze(parent, root)?;
        self.check_type(&expr_type)?;
        return Ok(expr_type);
    }

    fn analyze_arithmetic(
        &mut self,
        parent: SymbolNodeRef,
        root: SymbolNodeRef,
    ) -> Result<TypeSpecifier> {
        let result = matches!(
            self.value.as_ref(),
            Expression::ArrayInit(_) | Expression::ObjectInit(_)
        );
        if result {
            return Err(CompilerError::Semantic(String::from(
                "Unexpected expression",
            )));
        }

        let expr_type = self.value.analyze(parent, root)?;
        self.check_type(&expr_type)?;
        return Ok(expr_type);
    }

    fn analyze_not(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let result = matches!(
            self.value.as_ref(),
            Expression::ArrayInit(_) | Expression::ObjectInit(_)
        );
        if result {
            return Err(CompilerError::Semantic(String::from(
                "Unexpected expression",
            )));
        }

        let expr_type = self.value.analyze(parent, root)?;

        if let UnaryOperators::BitwiseNot = self.operator {
            return match expr_type {
                TypeSpecifier::Int | TypeSpecifier::Usize => Ok(expr_type),
                _ => Err(CompilerError::Semantic(String::from(
                    "Type not supported for the operation",
                ))),
            };
        }

        if let UnaryOperators::LogicalNot = self.operator {
            let TypeSpecifier::Bool = expr_type else {
            return Err(CompilerError::Semantic(String::from(
                    "Type not supported for the operation",
                )));
          };
        }
        return Ok(expr_type);
    }

    fn analyze_address(
        &mut self,
        parent: SymbolNodeRef,
        root: SymbolNodeRef,
    ) -> Result<TypeSpecifier> {
        let result = matches!(
            self.value.as_ref(),
            Expression::Identifier(_)
                | Expression::MemberAccess(_)
                | Expression::ArrayIndex(_)
                | Expression::Unary(_)
        );

        if !result {
            return Err(CompilerError::Semantic(String::from(
                "Unexpected expression",
            )));
        }

        let expr_type = self.value.analyze(parent, root)?;
        return Ok(TypeSpecifier::Pointer(Box::new(expr_type)));
    }

    fn analyze_pointer(
        &mut self,
        parent: SymbolNodeRef,
        root: SymbolNodeRef,
    ) -> Result<TypeSpecifier> {
        let result = matches!(
            self.value.as_ref(),
            Expression::Identifier(_)
                | Expression::MemberAccess(_)
                | Expression::ArrayIndex(_)
                | Expression::Unary(_)
        );
        if !result {
            return Err(CompilerError::Semantic(String::from(
                "Unexpected expression",
            )));
        }
        let expr_type = self.value.analyze(parent, root)?;
        if let TypeSpecifier::Pointer(type_specifier) = expr_type {
            return Ok(*type_specifier);
        }
        return Err(CompilerError::Semantic(String::from(
            "Expected pointer type",
        )));
    }

    fn check_type(&self, expr_type: &TypeSpecifier) -> Result<()> {
        let result = matches!(
            expr_type,
            TypeSpecifier::Int
                | TypeSpecifier::Usize
                | TypeSpecifier::Float
                | TypeSpecifier::Double
        );

        if !result {
            return Err(CompilerError::Semantic(String::from(
                "Value is not supported for the operator",
            )));
        }

        return Ok(());
    }
}
