# remcom `1.0.0`

A lightweight command-line utility to remove comments from C/C++ source files.

## Description

**remcom** ("remove comments") strips both single-line (`//`) and multi-line (`/* */`) comments from C/C++ source code while preserving comments inside string literals and character constants.

## Usage

```
remcom [options] <filename> [start_line] [end_line]
```

### Options

| Option | Description      |
| ------ | ---------------- |
| `-v`   | Show version     |
| `-h`   | Show help        |

### Arguments

| Argument     | Description                                      |
| ------------ | ------------------------------------------------ |
| `filename`   | Path to the source file                          |
| `start_line` | First line to process (default: 1)               |
| `end_line`   | Last line to process (default: end of file)      |

### Examples

Strip comments from the entire file:
```
remcom main.c
```

Strip comments from lines 10 to 20:
```
remcom main.c 10 20
```

Save output to a new file:
```
remcom main.c > cleaned.c
```

## Build

Requires a C99+ compiler. No external dependencies.

```
cc -Wall -Wextra -Wpedantic remcom.c -o remcom
```
