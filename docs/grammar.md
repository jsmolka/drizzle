# Grammar
The grammar uses the notation proposed in [Crafting Interpreters](http://www.craftinginterpreters.com/representing-code.html#enhancing-our-notation).

```
program           → declaration* EOF
```

## Declarations
A program is a series of declarations, which are the statements that bind new identifiers or any of the other statement types.
```
declaration       → defDecl
                  | varDecl
                  | statement
defDecl           → "def" function
varDecl           → "var" IDENTIFIER ( "=" expression )? NEWLINE
```

## Statements
The remaining statement rules produce side effects, but do not introduce bindings.
```
statement         → exprStmt
                  | whileStmt
                  | returnStmt
                  | noopStmt
                  | ifStmt
                  | continueStmt
                  | breakStmt
                  | blockStmt
exprStmt          → expression NEWLINE
whileStmt         → "while" expression block
returnStmt        → "return" expression? NEWLINE
noopStmt          → "noop" NEWLINE
ifStmt            → "if" expression block
                  ( "elif" expression block )*
                  ( "else" block )?
continueStmt      → "continue" NEWLINE
breakStmt         → "break" IDENTIFIER? NEWLINE
blockStmt         → "block" IDENTIFIER? block
block             → ":" NEWLINE INDENT declaration+ DEDENT
```

## Expressions
Expressions produce values. The precedence goes from high (bottom) to low (top).
```
expression        → assignment
assignment        → IDENTIFIER "=" assignment | or
or                → and ( "||" and )*
and               → bitOr ( "&&" bitOr )*
bitOr             → bitXor ( "|" bitXor )*
bitXor            → bitAnd ( "^" bitAnd )*
bitAnd            → equality ( "&" equality )*
equality          → comparison ( ( "!=" | "==" ) comparison )*
comparison        → bitShift ( ( ">" | ">=" | "<" | "<=" ) bitShift )*
bitShift          → term ( ( "<<" | ">>" | ">>>" ) term )*
term              → factor ( ( "-" | "+" ) factor )*
factor            → unary ( ( "/" | "//" | "*" | "**" | "%" ) unary )*
unary             → ( "!" | "-" | "~" ) unary | call
call              → primary ( "(" arguments? ")" )*
primary           → "true" | "false" | "null" | NUMBER | STRING | IDENTIFIER | "(" expression ")"
```

### Utilities
```
function          → IDENTIFIER "(" parameters? ")" block
parameters        → IDENTIFIER ( "," IDENTIFIER )*
arguments         → expression ( "," expression )*
```

## Lexical Grammar

### Numeric Literals
```
NUMBER            → FLOAT | INTEGER
FLOAT             → DECIMAL "." DIGIT+
INTEGER           → DECIMAL | BINARY | HEXADECIMAL
DECIMAL           → "0" | ( "1" ... "9" DIGIT* )
HEXADECIMAL       → "0x" ( "a" ... "f" | "A" ... "F" | DIGIT)+
BINARY            → "0b" ( "0" | "1" )+
```

### String Literals
```
STRING            → RAW_STRING | DEFAULT_STRING
RAW_STRING        → """ <any char> """
DEFAULT_STRING    → " <any char or escape sequence> "
```

In raw string literals, unescaped sequences are allowed and retained, except that three unescaped quotes in a row terminate the literal. In non-raw string literals, these escape sequences are recognized:

| Escape Sequence | Meaning               |
|:----------------|:----------------------|
| `\\`            | Backslash             |
| `\"`            | Double quote          |
| `\n`            | ASCII Linefeed        |
| `\r`            | ASCII Carriage Return |
| `\t`            | ASCII Horizontal Tab  |

### Identifiers
```
IDENTIFIER        → ALPHA ( ALPHA | DIGIT )*
ALPHA             → "a" ... "z" | "A" ... "Z" | "_"
DIGIT             → "0" ... "9"
```

The following identifiers are reserved as words:
- `block`
- `break`
- `continue`
- `def`
- `elif`
- `else`
- `false`
- `if`
- `noop`
- `null`
- `return`
- `true`
- `var`
- `while`
