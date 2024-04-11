use compiler::Compiler;

pub mod compiler;

fn main() {
    let compiler = Compiler::new("main.mb".to_owned());
    if let Err(err) = compiler.analyze() {
        println!("{}", err);
    }
}
