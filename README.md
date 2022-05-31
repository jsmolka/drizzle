# drizzle
A dynamic interpreted programming language.

## Syntax
drizzle might seem very familiar. I wanted to find out how parsing a whitespace-aware language works, and due to my lack of imagination, I ended up with something similar to Python. If I were to design another language, I would use braces to avoid the annoyances that come with whitespace awareness:
- What counts as an indentation?
- Can we mix spaces and tabs? If so, how many spaces are one tab?
- How do we define anonymous functions with multiple lines?

## Binaries
Binaries for Windows, Linux and macOS are available as [nightly](https://github.com/jsmolka/drizzle/actions/workflows/build.yml) and [release](https://github.com/jsmolka/drizzle/releases) builds.

## Build
Detailed build instructions can be found [here](BUILD.md).

## Tour
```python
# Primitives
var b = true
var i = 1
var f = 1.0
assert(b == i)
assert(i == f)
assert(f == b)

# Lists
var l = [1, 2, 3]
assert(l[0] == 1)
assert(l[-1] == 3)
assert(l == [1, 2] + [3])
assert(l.size() == 3)

# Maps
var m = {"key": "value", 1: 2}
assert(m["key"] == "value")
assert(m[1] == 2)
assert(m[2] == null)
assert(m.size() == 2)

# Functions
def fibonacci(n):
  if n < 2:
    return n
  return fibonacci(n - 2) + fibonacci(n - 1)

assert(fibonacci(10) == 55)

# Control flow
if true:
  noop
elif true:
  noop
else:
  noop

while false:
  noop

# Blocks and shadowing
var s = 0
block:
  var s = s
  assert(s == 0)
  s = 1
  assert(s == 1)
assert(s == 0)

block outer:
  while true:
    while true:
      break outer

# Iterators
for i in [0, 1, 2]:
  noop  # Iterate 0 to 2

for i in reverse([0, 1, 2]):
  noop  # Iterate 2 to 0

for i in range(0, 3, 1):
  noop  # Iterate 0 to 2

for i in reverse(0 .. 3):
  noop  # Iterate 2 to 0

# Classes
class Fibonacci:
  def init(n):
    this.n = n
  def calc():
    var a = 0
    var b = 1
    for i in 0 .. this.n:
      var c = a + b
      a = b
      b = c
    return a

var o = Fibonacci(10)
var c = o.calc
assert(o.n == 10)
assert(o["n"] == 10)
assert(c() == 55)
```
