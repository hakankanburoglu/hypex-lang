# If-Else Statement

If-Else statements are blocks of code that will be executed when a certain condition is met.

```
if condition:
    expressions
else:
    expressions
```

The Else statement is the code block that will run when the condition in the If statement is not met.

```
func main():
    var:int n
    >> "Enter a number : "
    << n
    if n > 0:
        >> "The number you entered is a positive number."
    else:
        >> "The number you entered is not a positive number."
```

The Else-If block functions as a combination of Else and If blocks. The Else-If block is defined with the `elif` keyword. Else-If block cannot be defined with `else` and `if` keywords.

```
func main():
    var:int n
    >> "Enter a number : "
    << n
    if n > 0:
        >> "The number you entered is a positive number."
    elif n < 0:
        >> "The number you entered is a negative number."
    else:
        >> "The number you entered is zero."
```

[Index](index.md) | [<< Back](9_constants.md) | [Next >>](11_switch_statement.md)