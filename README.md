# Tron Programming Language

Tron is a statically-typed, strongly-typed programming language designed for high-performance data-intensive applications. It compiles to machine code through LLVM and aims to offer a simple, expressive syntax that supports modern programming paradigms.

The long-term vision for Tron is to become a data-native programming language, incorporating seamless integration of tensors and tensor operations at the language level. This design philosophy emphasizes optimal performance and efficiency for data-heavy applications.

## First Phase Goals

During the first phase of development, the primary focus is to create a rudimentary calculator that supports integers and floating-point numbers, while also laying the groundwork for the language's overall design.

## Preview

```go

var foo = 3;

var bar: float = 4.42;

func myfunc(a: int, b: float): float {
    var c = as_float(a) * b;
    return c;
}

var baz = myfunc(3) * (as_float(foo) + bar);

if (foo >= 32) {
    print(foo);
} else if (baz >= 12) {
    print(bar);
} else {
    print(baz);
}

var i = 0;
while (i < 10) {
    print(i);
    i = i + 1;
}

```

## First Phase Progress

- ~~Initial lexer: Tokenizing a sufficient number of tokens to support the first phase.~~
- ~~Initial parser: Parsing the bare minimum statements required for the first phase.~~
- ~~Scope handling: Maintaining a scope-based symbol table to track functions, variables, and types.~~
- ~~Expression auto type resolution over type inference.~~
- ~~Conditional operators.~~
- ~~Parse time semantic checks.~~
- ~~Consider a generic backend to support both llvm~~ and wasm.
- ~~Initial LLVM emit and compilation to a final binary object, ready for linking with external executables.~~
- Perform a memory review and cleanup to ensure proper handling of unfreed allocations in the source code.
