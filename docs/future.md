# Todo

## Functions
- type()
- print()
- assert()
- conversion functions that return null on error

## Classes
- $construct
- $string
- $iterate

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

## Error Handling
Doesn't seem to be necessary now. Consider adding basic `try/catch` block.

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
