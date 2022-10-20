# drizzle
A dynamic interpreted programming language.

<sup>(it's not Python, I swear)</sup>

## Usage
```
usage:
  drizzle [-h] [-a] <file>

keyword arguments:
  -h, --help    print help
  -a, --ast     print ast

positional arguments:
  file          script file
```

Additional arguments can be passed after `<file>` and later be accessed through the `arguments` function.

## Examples
```python
class Point:
  def init(x, y):
    this.x = x
    this.y = y
  def cross(other):
    return this.x * other.y - this.y * other.x

# Stokes' theorem
def area(polygon):
  var s = 0
  for i in 0 .. len(polygon):
    s = s + polygon[i - 1].cross(polygon[i])
  return s / 2

var triangle = [Point(0, 0), Point(4, 4), Point(0, 4)]

assert(area(triangle) == 8.0)
```

See [examples](/examples) for more.

## Binaries
Binaries for Windows, Linux and macOS are available as [nightly](https://nightly.link/jsmolka/drizzle/workflows/build/master) and [release](https://github.com/jsmolka/drizzle/releases) builds.

## Build
Detailed build instructions can be found [here](BUILD.md).

## Credit
Thanks to Robert Nystrom for [Crafting Interpreters](https://www.craftinginterpreters.com).
