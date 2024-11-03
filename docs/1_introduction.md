# Introduction

Hypex is a multi-paradigm, general-purpose programming language that generally uses static systems but also offers dynamic system support, offers a platform-independent and C-like experience, and has a readable and simple syntax. It pays attention to the traditional C-type syntax and aims to provide it in a more modern, simple and readable format, without causing any loss of performance. The main purpose of the language is to provide easy syntax while not losing readability and information required by the compiler. Under the promise of simple syntax in programming languages, a large workload is placed on compilers, which significantly extends the compilation time and causes performance losses. At this point, Hypex provides simple syntax and uses only as simple a syntax as necessary, taking into account compiler performance. It aims to balance the benefit relationship between developer and compiler.

## Indentation Style

Hypex uses block indentation. It does not use end-of-line semicolons. The main program code is written into the main function.

```
func main():
    var:int n
    << n
    for i : range(0, n):
        >> n + "\n"
```

But it also optionally supports the classic brace-blocking style. In fact, optionally, semi-colon can be placed at the end of the lines.

```
func main() {
    var:int n
    << n
    for i : range(0, n)
        >> n + "\n"
}
```

```
func main() {
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            >> i + " is even number";
        } else {
            >> i + " is not even number";
        }
    }
}
```

[Index](index.md) | [Next >>](2_hello_world.md)