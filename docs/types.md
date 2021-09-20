# Types

| Rank | Type  | Representation    |
|------|-------|-------------------|
|      | null  |                   |
| 1    | bool  | bool              |
| 2    | int   | 64-bit signed int |
| 3    | float | 64-bit float      |

Operands in mixed operations are promoted to the highest ranked operand type. An exception to this rule is division, where the result will always be a float.
