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
    var:Number num = Number.ONE
    >> num.value // 1
    num = Number[1]
    >> num.value // 2
    num++
    >> num.value // 3

```

[Index](index.md) | [<< Back](15_functions.md) | [Next >>](17_structures.md)