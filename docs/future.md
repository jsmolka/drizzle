# Todo
- Foreign Function Interface
- Iterators, mix yield and return (to stop)
- Tuples, destructuring
- Rename scanner to tokenizer
- Fix max int parsing
- Late bind top level functions and classes
- Using < for chunk.line() seems wrong, use pc - 1 instead?
- Precedence assignment for parens ok?
- Map optimized for integers
- Make print a function with no fixed arity
- Handle chained comparisons

## Exceptions
- [Zero cost](https://bugs.python.org/issue40222)
- Show call stack

## Functions
- type(), classes should return "object"
- print()
- assert()
- format()
- Conversion functions that return null on error

## Closures
- Efficient implementation keywords: "closure conversion", "lambda lifting"
- AST approach: http://www.craftinginterpreters.com/closures.html#upvalues

## Classes
- $ (constructor)
- $in, $for, $str, $[op]

```dz
class Point:
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

## AST
- C++ AST example:  https://github.com/wiz-lang/wiz/tree/master/src/wiz

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
