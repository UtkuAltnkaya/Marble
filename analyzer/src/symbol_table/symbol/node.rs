use crate::ast::type_specifier::TypeSpecifier;

#[derive(Debug)]
pub enum NodeTypes {
    Variable(VariableNode),
    Function(FunctionNode),
    Block,
    Global,
    Struct,
    StructItem(VariableNode),
    Enum,
    EnumItem,
}

#[derive(Debug)]
pub struct VariableNode {
    pub type_specifier: TypeSpecifier,
}

#[derive(Debug)]
pub struct FunctionNode {
    pub return_type: TypeSpecifier,
    pub params: Vec<TypeSpecifier>,
}

impl VariableNode {
    pub fn new(type_specifier: TypeSpecifier) -> Self {
        Self { type_specifier }
    }
}

impl FunctionNode {
    pub fn new(return_type: TypeSpecifier, params: Vec<TypeSpecifier>) -> Self {
        Self {
            return_type,
            params,
        }
    }
}
