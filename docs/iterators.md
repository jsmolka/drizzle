# Iterators

## Planned
Iterators behave much like functions and use the `yield` keyword instead of `return`. They offer the same default and named argument syntax like functions as their counter parts. They are computed during each iteration. They are accessed by for loops an their usage is signaled by the `in` keyword.

### Standard Syntax
```
iterator range(to):
  for i = 0; i < to; i++:
    yield i

for i in range(10):
  pass
```

### Class Iterators
Classes can define a default `iterate` iterator.
```
class List:
  constructor():
    this.array = [1, 2, 3]

  iterator iterate():
    for i = 0; i < this.array.len; i++:
      yield this.array[i]

  iterator reversed():
    for i = this.array.len - 1; i > -1; i--:
      yield this.array[i]

var list = List()
for i in list:
  pass

for i in list.reversed():
  pass
```
