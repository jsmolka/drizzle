# Loops

## Planned
- break
- continue

### While Loops
```
while condition:
  noop
```

### For Loops
```
iterator range(min, max, step=1):
  while min < max:
    yield min
    min += step
```

```
# Range [0, 9]
for i in range(0, 10):
  noop
```

```
# Range [0, -9]
for i in range(0, -10, -1):
  noop
```

```
# Range [0, 9]
for i in 0 .. 10:
  noop
```

## Maybe
Automatic determination of step in `..`.
```
# Range [0, -9]
for i in 0 .. -10:
  noop
```
