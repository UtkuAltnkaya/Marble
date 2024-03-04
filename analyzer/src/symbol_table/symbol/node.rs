use crate::ast::type_specifier::TypeSpecifier;

#[derive(Debug)]
pub enum NodeTypes {
    Variable(VariableNode),
    Function(FunctionNode),
    Block,
    Global,
    Struct,
    StructField(VariableNode),
    Enum,
    EnumItem,
}

impl NodeTypes {
    pub fn variable(&self) -> &VariableNode {
        return match self {
            NodeTypes::Variable(var) | NodeTypes::StructField(var) => var,
            _ => unreachable!("Something went wrong"),
        };
    }

    pub fn function(&self) -> &FunctionNode {
        return match self {
            NodeTypes::Function(function) => function,
            _ => unreachable!("Something went wrong"),
        };
    }
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
