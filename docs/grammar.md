# Grammar
The grammar uses the notation proposed in [Crafting Interpreters](http://www.craftinginterpreters.com/representing-code.html#enhancing-our-notation).

## Declarations
A program is a series of declarations, which are the statements that bind new identifiers or any of the other statement types.
```
declaration       → varDecl
                  | statement ;
varDecl           → "var" IDENTIFIER ( "=" expression )? NEWLINE ;
```

## Statements
The remaining statement rules produce side effects, but do not introduce bindings.
```
statement         → exprStmt
                  | assertStmt
                  | ifStmt
                  | printStmt
                  | block ;

exprStmt          → expression NEWLINE ;
assertStmt        → "assert" expression NEWLINE ;
ifStmt            → "if" expression BLOCK_BODY
                    ( "elif" expression BLOCK_BODY )*
                    ( "else" BLOCK_BODY )? ;
printStmt         → "print" expression NEWLINE ;
block             → "block" BLOCK_BODY ;
BLOCK_BODY        → ":" NEWLINE INDENT declaration+ DEDENT
```

## Expressions
Expressions produce values. The precedence goes from high/bottom to low/top.
```
expression        → assignment ;
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
- `continue`
- `def`
- `elif`
- `else`
- `false`
- `for`
- `if`
- `in`
- `noop`
- `null`
- `return`
- `true`
- `var`
- `while`
