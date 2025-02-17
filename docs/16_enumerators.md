# Enumarations

```
enum Number:
    ONE = 1,
    TWO = 2,
    THREE = 3
```

```
enum Number:
    ONE = 1,
    TWO,
    THREE

func main():
    var:Number n = Number.ONE
    >> n.value + "\n" // 1
    n = Number[1]
    >> n.value + "\n" // 2
    n++
    >> n.value + "\n" // 3

```

[Index](index.md) | [<< Back](15_functions.md) | [Next >>](17_structures.md)