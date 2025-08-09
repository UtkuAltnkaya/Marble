# Marble

Marble is a statically typed compiler for a custom programming language, developed in C++ with a modular frontend-backend design. It is built with a focus on extensibility, and low-level language design.

## Features

- Custom language syntax with support for:
  - Functions, structs, enums, variables
  - Generics with monomorphization
  - Type inference and basic type checking
  - Desugaring (e.g., method calls into function calls)
- Full compiler pipeline: lexer, parser, semantic analyzer, and LLVM-based code generation

## Example

```
struct Rectangle {
    width: int,
    height: int,
}

impl Rectangle {
    pub fn init(width: int, height: int) -> Rectangle {
        return Rectangle { width, height };
    }

    pub fn (r: Rectangle*) area() -> int {
        return r->width + r->height;
    }

    pub fn (r: Rectangle*) perimeter() -> int {
        return (2 * r->width) + (2 * r->height);
    }
}

fn max<T>(x: T, y: T) -> T {
    if (x > y) {
        return x;
    }
    return y;
}


fn main() -> int {
    let r1 = Rectangle::init(10,5);
    let r2 = Rectangle::init(9,6);

    let area1 = r1.area();
    let area2 = r2.area();

    let max_area = max<int>(area1,area2);

    let max_perimeter = max<int>(r1.perimeter(), r2.perimeter());

    return 0;
}

```

## Getting Started

#### Prerequisites

- C++23 compiler
- CMake
- LLVM (linked during code generation)

#### Build Instructions

```bash
git clone https://github.com/UtkuAltnkaya/Marble.git
cd Marble
mkdir build
cmake -B build
cmake --build ./build
```

#### Running

```bash
./build/src/Marble/marble -f ./example/main.mb
```

This will parse, analyze, and eventually generate output code (IR backend in development).

### Project Structure

- `src/` - Core compiler source code
  - `Marble` - CLI executable entry point
  - `Lexer/`, `Parser/`, `SemanticAnalyzer/`, `Codegen/` - Compiler stages
  - `Ast/` - Abstract Syntax Tree (AST) definitions
  - `SymbolTable/` - Scope and symbol management
  - `ErrorSystem/` - Error and diagnostic reporting
  - `Utils/` - Utility functions and helpers
- `example/` - Sample programs written in the Marble language
- `tests/` – Unit tests for internal modules
- `grammar.txt` – Language grammar (EBNF-style)
- `CMakeLists.txt`, `Makefile` – Build configuration files

### Language Design

- Inspired by Rust, C/C++, and Go
- Struct and impl definitions
- Monomorphized generics
- Manual memory model (C-like)
- Desugaring method calls into top-level functions

### Development Status

- ✅ Lexer and Parser
- ✅ Semantic Analysis with type checking and generics
- ⚙️ Code generation using LLVM
- ⚙️ Object file generation
- ⚙️ Linking with libc
- ⚙️ Multiple compilation unit
