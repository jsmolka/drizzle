# Iterators

## Planned
Iterators behave much like functions and use the `yield` keyword instead of `return`. They offer the same default and named argument syntax like functions as their counter parts. They are computed during each iteration. They are accessed by for loops an their usage is signaled by the `in` keyword.

### Basic Iterator
```
iterator range(to):
  for i = 0; i < to; i++:
    yield i

for i in range(10):
  pass
```

### Class Default Iterators
Classes can define a default `iterate` iterator.
```
class Counter:
  iterator iterate():
    var i = 0
    while true:
      yield i
      i++

for i in Counter():
  noop
```
