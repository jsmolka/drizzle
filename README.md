# drizzle
A dynamic interpreted programming language.

<sup>(it's not Python, I swear)</sup>

## Examples
That's how selection sort would be implemented:

```python
def selection_sort(array):
  for i in 0 .. array.size():
    var minimum = i
    for j in i + 1 .. array.size():
      if array[j] < array[minimum]:
        minimum = j
    var temp = array[i]
    array[i] = array[minimum]
    array[minimum] = temp
  return array
```

For more see [`/examples/*.dz`](/examples/).

## Binaries
Binaries for Windows, Linux and macOS are available as [nightly](https://nightly.link/jsmolka/drizzle/workflows/build/master) and [release](https://github.com/jsmolka/drizzle/releases) builds.

## Build
Detailed build instructions can be found [here](BUILD.md).

## Credit
Thanks to Robert Nystrom for [Crafting Interpreters](https://www.craftinginterpreters.com).
