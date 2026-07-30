# remcom `1.1.0`

A lightweight command-line utility to remove comments from source files.

## Description

**remcom** ("remove comments") strips both single-line and multi-line comments from source code while preserving comments inside string literals and character constants.

By default it targets C/C++ style comments (`//` and `/* */`). Pass `-l python` to strip Python-style comments (`#` and triple-quoted `"""` blocks) instead.

## Usage

```
remcom [options] <filename> [start_line] [end_line]
```

### Options

| Option       | Description                          |
| ------------ | ------------------------------------- |
| `-v`         | Show version                          |
| `-h`         | Show help                             |
| `-l <lang>`  | Set comment style for language (currently supports `python`; defaults to C-style) |

### Arguments

| Argument     | Description                                      |
| ------------ | ------------------------------------------------ |
| `filename`   | Path to the source file                          |
| `start_line` | First line to process (default: 1)               |
| `end_line`   | Last line to process (default: end of file)      |

### Examples

Strip C-style comments from the entire file:
```
remcom main.c
```

Strip comments from lines 10 to 20:
```
remcom main.c 10 20
```

Strip Python-style comments:
```
remcom -l python script.py
```

Save output to a new file:
```
remcom main.c > cleaned.c
```

> **Note:** flags should be passed before the filename (e.g. `remcom -l python script.py`, not `remcom script.py -l python`) for consistent behavior across platforms.

## Build

Requires a C99+ compiler. No external dependencies.

```
cc -Wall -Wextra -Wpedantic remcom.c -o remcom
```