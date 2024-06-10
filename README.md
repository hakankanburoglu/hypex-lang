# The Hypex Programming Language

## Introduction

Hypex is a multi-paradigm, general-purpose programming language that generally uses static systems but also offers dynamic system support, offers a platform-independent and C-like experience, and has a readable and simple syntax. It pays attention to the traditional C-type syntax and aims to provide it in a more modern, simple and readable format, without causing any loss of performance. The main purpose of the language is to provide easy syntax while not losing readability and information required by the compiler. Under the promise of simple syntax in programming languages, a large workload is placed on compilers, which significantly extends the compilation time and causes performance losses. At this point, Hypex provides simple syntax and uses only as simple a syntax as necessary, taking into account compiler performance. It aims to balance the benefit relationship between developer and compiler.

## Examples

```
func main():
    >> "Hello, World!"
```

```
func main():
    var:string name
    >> "Enter your name : "
    << name
    >> "Hello, " + name + "!"
```
