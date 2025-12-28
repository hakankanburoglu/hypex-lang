<br><div align="center"><img src="docs/assets/hypex-logo.png" alt="Hypex Logo" width=250px/></div><br>

# The Hypex Programming Language

## Introduction

Hypex is a statically typed, multi-paradigm, general-purpose programming language focused on performance, readability, and compiler efficiency. It provides a platform-independent, C-like development experience with a clean and modern syntax.

The language is designed to strike a balance between simplicity and expressiveness. It avoids hiding complexity behind heavy compiler logic and instead favors explicit and predictable syntax, balancing developer convenience with compiler simplicity.

It aims to deliver readable and expressive code without sacrificing performance or low-level control, maintaining a clear and transparent relationship between the programmer and the compiler.

## Examples

### Hello World

```
func main():
    >> "Hello, World!"
```

###  Input/Output

```
func main():
    var:string name
    >> "Enter your name : "
    << name
    >> "Hello, " + name + "!"
```

### Control Flow

```
func main():
    for i : 0..20:
        if i % 2 == 0:
            >> i + " is even number\n"
        else:
            >> i + " is odd number\n"
```

### Functions

```
func:int add(int a, int b):
    return a + b

func main():
    >> add(1, 2)
```

## License

MIT License
