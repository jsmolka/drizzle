# Functions

## Planned
Make sure to limit the function argument count (arity). Raise a syntax error if the number of non-default arguments doesn't match the number of passed arguments.

### Basic Function
```
function add(a, b):
  return a + b

add(1, 2)  # Returns 3
```

### Anonymous Function
<!-- Todo: rethink syntax https://nim-lang.org/docs/manual.html#procedures-anonymous-procs -->
```
function call(func):
  return func(10)

call(function (value):
  return value)

call(function (value):
  return value
)
```

### Default Arguments
All default arguments must be at the end of the argument list or a syntax error is raised.
```
function add(a, b=2):
  return a + b

add(1)  # Returns 3
add(1, 3)  # Returns 4
```

### Named Arguments
Default arguments can be passed via their names.
```
function add(a, b=2, c=3):
  return a + b + c

add(1, c=4)  # Returns 7
```

The order doesn't matter in that case.
```
function add(a, b=2, c=3):
  return a + b + c

add(1, c=4, b=5)  # Returns 10
```
