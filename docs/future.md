# Todo
- Foreign Function Interface
- Should global be bound late like in Python?
- Iterators, mix yield and return (to stop)
- Tuples, destructuring
- Rename scanner to tokenizer

## Exceptions
- [Zero cost](https://bugs.python.org/issue40222)

## Functions
- type(), classes should return "object"
- print()
- assert()
- format()
- conversion functions that return null on error

## Classes
- Define variables beforehand
  - Don't allow assignment to undefined stuff
  - Can this be used for optimizing?
- $ (constructor)
- $in, $for, $str, $[op]

```dz
class Point:
  var x
  var y

  def $(x, y):
    this.x = x
    this.y = y

  def $str():
    return format("{},{}", this.x, this.y)


var p = Point(1, 1)
print(p)
```

## Scanning
- Brace aware scanning (line breaks inside braces, brackets, etc. are ignored)
- Should `0xDEADreturn` result in two valid tokens?

## AST
Generate an AST and perform certain optimization/syntax passes:
- Constant folding, type checking and range checks for numbers
- Dead code elimination
- Loop invariation
- Desugar certain constructs (`var x` into `var x = null`)
- Basic syntax checking

## Switch Case
```dz
switch x + y:
  case 1:
    noop
  case _:
    noop
  case 1 + 1:
    noop
```

Requires a proper AST instead of a single pass compiler. Perform constant folding pass first and require constant `case` labels. Consider allowing dynamic variables. Reorder above code into:

```dz
switch x + y:
  case 1:
    noop
  case 2:
    noop
  case _:
    noop
```

Desugar into:
```
var $i = x + y
if $i == 1:
  noop
elif $i == 2:
  noop
else:
  noop
```

## Operators
- Operation equals
- Pre and post increment/decrement (are these language conform?)

## Performance
- Direct threaded code
- Jump tables
- Computed goto
- Register based bytecode
- Keep IP in CPU register, no member variable
- add opcodes for frequent numbers -1, 0, 1, 2
- Optimize string literals, don't intern everything