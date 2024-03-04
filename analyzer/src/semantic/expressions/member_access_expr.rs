use crate::{
    ast::{
        access_specifier::AccessSpecifier,
        expressions::{
            fn_call_expr::FnCallExpression, identifier_expression::IdentifierExpression,
            member_access_expr::MemberAccessExpression, Expression,
        },
        identifier::Identifier,
        type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    lexer::token_type::TokenType,
    semantic::AstAnalyze,
    symbol_table::symbol::{
        data::Access,
        iter::{SymbolIterator, ToIter},
        node::NodeTypes,
        SymbolNodeRef,
    },
};

impl AstAnalyze for MemberAccessExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        self.check_obj()?;
        let object = self.object.analyze(parent.clone(), root.clone())?;
        let struct_name = match object {
            TypeSpecifier::UserDefine(identifier) => self.check_user_define(identifier)?,
            TypeSpecifier::Pointer(pointer) => self.check_pointer(*pointer)?,
            _ => {
                return Err(CompilerError::Semantic(String::from(
                    "Member access only can use with user define type",
                )))
            }
        };
        return self.check_property(struct_name.as_ref(), parent, root);
    }
}

impl MemberAccessExpression {
    fn check_obj(&self) -> Result<()> {
        let result = matches!(
            self.object.as_ref(),
            Expression::Binary(_)
                | Expression::Assignment(_)
                | Expression::ArrayInit(_)
                | Expression::Primitive(_)
                | Expression::MemberAccess(_)
        );
        if result {
            return Err(CompilerError::Semantic(String::from(
                "Invalid object expression",
            )));
        }
        return Ok(());
    }

    fn check_user_define(&self, id: Identifier) -> Result<Identifier> {
        if TokenType::Dot == self.access {
            return Ok(id);
        }
        return Err(CompilerError::Semantic(String::from(
            "Use dot('.') operator to access member",
        )));
    }

    fn check_pointer(&self, type_specifier: TypeSpecifier) -> Result<Identifier> {
        let TypeSpecifier::UserDefine(identifier) = type_specifier else {
          return Err(CompilerError::Semantic(String::from("Member access only can use with user define type")));
        };

        if TokenType::Arrow == self.access {
            return Ok(identifier);
        }

        return Err(CompilerError::Semantic(String::from(
            "Use arrow('->') operator to access member with pointer type",
        )));
    }

    fn check_property(
        &mut self,
        name: &str,
        parent: SymbolNodeRef,
        root: SymbolNodeRef,
    ) -> Result<TypeSpecifier> {
        let iter = root
            .iter()
            .struct_sym(name)
            .ok_or_else(|| CompilerError::Semantic(String::from("Cannot find the struct")))?;

        let result = matches!(
            self.property.as_ref(),
            Expression::Identifier(_) | Expression::FnCall(_) | Expression::MemberAccess(_)
        );

        if !result {
            return Err(CompilerError::Semantic(String::from(
                "Invalid property expression",
            )));
        }

        match self.property.as_mut() {
            Expression::FnCall(fn_call) => Self::analyze_fn(fn_call, parent, iter),
            Expression::Identifier(identifier) => {
                Self::analyze_identifier(identifier, parent, iter)
            }
            Expression::MemberAccess(member_access) => {
                Self::analyze_member_access(member_access, iter)
            }
            _ => Err(CompilerError::Semantic(String::from(
                "Invalid property expression",
            ))),
        }
    }

    fn analyze_fn(
        fn_call: &mut FnCallExpression,
        parent: SymbolNodeRef,
        iter: SymbolIterator,
    ) -> Result<TypeSpecifier> {
        let node = iter
            .function(fn_call.fn_name.identifier()?.id.as_ref())
            .ok_or_else(|| CompilerError::Semantic(String::from("Cannot find the function")))?
            .find();

        Self::check_access_specifier(&node.borrow().data.access, parent.clone())?;
        // It is in the struct scope now
        let root = iter.find();
        return fn_call.analyze(parent, root);
    }

    fn analyze_identifier(
        identifier: &IdentifierExpression,
        parent: SymbolNodeRef,
        iter: SymbolIterator,
    ) -> Result<TypeSpecifier> {
        let node = iter
            .struct_field(identifier.id.as_ref())
            .ok_or_else(|| CompilerError::Semantic(String::from("Cannot find the struct field")))?
            .find();

        let binding = node.borrow();

        Self::check_access_specifier(&binding.data.access, parent)?;

        let type_specifier = &binding.data.node_type.variable().type_specifier;
        return Ok(type_specifier.clone());
    }

    fn analyze_member_access(
        _member_access: &mut MemberAccessExpression,
        _iter: SymbolIterator,
    ) -> Result<TypeSpecifier> {
        todo!()
        // return Ok(TypeSpecifier::Void);
    }

    fn check_access_specifier(access: &Access, parent: SymbolNodeRef) -> Result<()> {
        let Access::AccessSpecifier(access_specifier) = access else { unreachable!("Something went wrong!") };

        if let AccessSpecifier::Public = access_specifier {
            return Ok(());
        }
        // Parent always will be a function or method because it can only be called from those
        // If parent's parent is Global then it cannot access the property
        // If parent's parent is Struct then it can access the private property
        let node = unsafe { parent.iter().parent().unwrap_unchecked() }.find();
        let binding = node.borrow();

        if let NodeTypes::Global = binding.data.node_type {
            return Err(CompilerError::Semantic(String::from("Property is private")));
        }

        return Ok(());
    }
}
