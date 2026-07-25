# remcom

A lightweight command-line utility to remove comments from C/C++ source files.

## Description

**remcom** ("remove comments") strips both single-line (`//`) and multi-line (`/* */`) comments from C/C++ source code while preserving comments inside string literals and character constants.

## Usage

```
remcom <filename> [start_line] [end_line]
```

### Arguments

| Argument     | Description                                      |
| ------------ | ------------------------------------------------ |
| `filename`   | Path to the source file                          |
| `start_line` | First line to process (default: 1)               |
| `end_line`   | Last line to process (default: end of file)      |

### Examples

Remove comments from the entire file:
```
remcom test.c
```

Remove comments from line 10 onward:
```
remcom test.c 10
```

Remove comments from lines 5 to 20:
```
remcom test.c 5 20
```

Redirect output to a new file:
```
remcom test.c > cleaned.c
```

## Build

Compile with any C99+ compiler (no external dependencies):

```
gcc -Wall -Wextra -Wpedantic source.c -o remcom
```

## License

MIT
