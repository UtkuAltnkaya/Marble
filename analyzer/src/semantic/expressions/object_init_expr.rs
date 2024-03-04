use crate::{
    ast::{
        expressions::object_init_expr::{Field, ObjectInitExpression},
        type_specifier::TypeSpecifier,
    },
    error::{CompilerError, Result},
    semantic::AstAnalyze,
    symbol_table::symbol::{
        iter::{IterNodeTypes, ToIter},
        SymbolNodeRef,
    },
};

impl AstAnalyze for ObjectInitExpression {
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let struct_name = self.object.identifier()?;

        let iter = root
            .iter()
            .struct_sym(struct_name.id.as_ref())
            .ok_or_else(|| CompilerError::Semantic(String::from("Cannot find the struct")))?;

        let len = iter.count(IterNodeTypes::StructField);
        let node = iter.find();

        if self.fields.len() > len {
            return Err(CompilerError::Semantic(String::from("Too many fields")));
        }
        if self.fields.len() < len {
            return Err(CompilerError::Semantic(String::from("Missing fields")));
        }

        for field in self.fields.iter_mut() {
            field.analyze(node.clone(), parent.clone())?;
        }

        return Ok(TypeSpecifier::UserDefine(struct_name.id.clone()));
    }
}

impl AstAnalyze for Field {
    // Parent is the struct itself. Like Person, Student
    // Root is the the parent where the object init expression is called.
    fn analyze(&mut self, parent: SymbolNodeRef, root: SymbolNodeRef) -> Result<TypeSpecifier> {
        let field_node: SymbolNodeRef = parent
            .iter()
            .struct_field(self.name.as_ref())
            .ok_or_else(|| {
                CompilerError::Semantic(format!(
                    "Cannot find the struct field named {}",
                    self.name.as_ref()
                ))
            })?
            .find();

        //Since root is the parent
        //Since the real root is lost and parent is the struct which scope is global. It's parent is the root that is needed.
        let type_specifier = self.value.analyze(root, unsafe {
            parent.iter().parent().unwrap_unchecked().find()
        })?;

        let binding = field_node.borrow();
        let variable = binding.data.node_type.variable();

        if variable.type_specifier != type_specifier {
            return Err(CompilerError::Semantic(String::from(
                "Struct type and expression types do not matches",
            )));
        }

        return Ok(type_specifier);
    }
}
