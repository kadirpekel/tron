# Tron

Tron is a statically-typed, strongly-typed programming language that compiles to machine code through LLVM. The language aims to provide a simple and expressive syntax, with support for modern programming paradigms.

The ultimate goal of Tron is to become a data-native programming language, seamlessly integrating support for tensors and tensor operations at the language level. This design philosophy prioritizes maximum performance and efficiency for data-intensive applications.

## First phase goals

The first phase of development focuses on a calculator with support for integers and floating-point numbers while featuring a significant set of language features.

## Preview

```typescript

var foo = 3;

var bar: float = 4.42;

def myfunc(a: int, b: float = 0.5): float {
    var c = a * b;
    return c;
}

var baz = myfunc(3) * (foo + bar);

```
