# Grammar
The grammar uses the notation proposed in [Crafting Interpreters](http://www.craftinginterpreters.com/representing-code.html#enhancing-our-notation).

## Expressions
Expressions produce values. The precedence goes from high/bottom to low/top.
```
expression        → primaryExpr
primaryExpr       → "true" | "false" | "null" | "this"
                  | NUMBER | STRING | IDENTIFIER | "(" expression ")" ;
```

## Lexical Grammar

### Numeric Literals
```
NUMBER            → FLOAT | INTEGER ;
FLOAT             → DECIMAL "." DIGIT+ ;
INTEGER           → DECIMAL | BINARY | HEXADECIMAL ;
DECIMAL           → "0" | ( "1" ... "9" DIGIT* ) ;
HEXADECIMAL       → "0x" ( "a" ... "f" | "A" ... "F" | DIGIT)+ ;
BINARY            → "0b" ( "0" | "1" )+ ;
```

### String Literals
```
STRING            → RAW_STRING | ESCAPED_STRING ;
RAW_STRING        → """ <any char> """ ;
ESCAPED_STRING    → " <any char or escape sequence> " ;
```

In raw string literals, unescaped sequences are allowed and retained, except that three unescaped quotes in a row terminate the literal. In non-raw string literals, these escape sequences are recognized:

| Escape Sequence | Meaning               |
|:----------------|:----------------------|
| `\\`            | Backslash             |
| `\"`            | Double quote          |
| `\a`            | ASCII Bell            |
| `\b`            | ASCII Backspace       |
| `\f`            | ASCII Formfeed        |
| `\n`            | ASCII Linefeed        |
| `\r`            | ASCII Carriage Return |
| `\t`            | ASCII Horizontal Tab  |
| `\v`            | ASCII Vertical Tab    |

### Identifiers
```
IDENTIFIER        → ALPHA ( ALPHA | DIGIT )* ;
ALPHA             → "a" ... "z" | "A" ... "Z" | "_" ;
DIGIT             → "0" ... "9" ;
```

The following identifiers are used as reserved words:
- `break`
- `class`
- `continue`
- `elif`
- `else`
- `extends`
- `false`
- `for`
- `function`
- `if`
- `in`
- `iterator`
- `noop`
- `null`
- `return`
- `super`
- `this`
- `true`
- `var`
- `while`
- `yield`
