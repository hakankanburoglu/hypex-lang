# Variables

Variables are locations that store and represent data.

Variables are defined with the `var` keyword.

```
var:type name = value
```

Since variable names are identifier, identifier naming conventions apply when naming variables. (See: [Identifiers](7_identifiers.md))

```
var:int n = 1
```

Variables can be defined without assignment. This is called variable declaration.

```
var:int n
```

If an assignment has been made, a variable can be defined without specifying an explicit type. The type of the variable is inferred at compile time based on the assigned value.

```
var n = 1
```

If you want to define a variable that does not have a permanent type to which different types of values will be assigned, objects should be used. (See: [Objects](#))

More than one variable can be defined in the same statement.

```
var:int id, count, size
```

There are two different types of assignment in multiple variable definition.

1. A single value is declared in the assignment. Thus, this value is assigned to all variables.
2. The values of all variables are assigned one by one, separated by commas. <u>The number of variables defined and the number of values written must be the same. Each variable must match its value.</u>

```
var:int id, count, size = 1
// id = 1, count = 1, size = 1
```

```
var:int id, age, salary = 1, 20, 1000
// id = 1, age = 20, salary = 1000
```

[Index](index.md) | [<< Back](7_identifiers.md) | [Next >>](9_constants.md)