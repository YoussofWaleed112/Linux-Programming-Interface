# Linux-Programming-Interface

# Unix Utilities in C

A collection of simple Unix command-line utilities implemented in C, mimicking the behavior of standard Unix tools.

---

## Utilities

| Utility | Description |
|---------|-------------|
| `pwd`   | Print the current working directory |
| `echo`  | Print arguments to standard output |
| `cp`    | Copy a file to a destination |
| `mv`    | Move/rename a file to a destination |

---

## Files

```
unix-utilities/
├── pwd.cpp
├── echo.cpp
├── cp.cpp
├── mv.cpp
└── README.md
```

> Files use the `.cpp` extension due to a platform limitation, but are written in pure C.

---

## Compilation

Each utility is compiled separately using `gcc`:

```bash
gcc -o pwd  pwd.cpp
gcc -o echo echo.cpp
gcc -o cp   cp.cpp
gcc -o mv   mv.cpp
```

Or compile all at once:

```bash
for f in pwd echo cp mv; do gcc -o $f $f.cpp; done
```

---

## Usage & Example Output

### pwd
Prints the absolute path of the current working directory.

```bash
$ ./pwd
/home/user/unix-utilities
```

---

### echo
Prints all arguments to stdout, space-separated, with a trailing newline.

```bash
$ ./echo Hello World
Hello World

$ ./echo
(blank line)
```

---

### cp
Copies a source file to a destination path. Creates the destination if it does not exist; truncates it if it does. Preserves the source file's permissions.

```bash
$ echo "Hello" > file.txt
$ ./cp file.txt /tmp/file_copy.txt
$ cat /tmp/file_copy.txt
Hello
```

Error case:
```bash
$ ./cp nonexistent.txt /tmp/out.txt
cp: cannot open source: No such file or directory
```

---

### mv
Moves a file to a new location or renames it. Uses `rename()` for same-filesystem moves (atomic, instant). Falls back to copy + delete for cross-device moves.

```bash
$ echo "Hello" > /tmp/file.txt
$ ./mv /tmp/file.txt /tmp/new_name.txt
$ cat /tmp/new_name.txt
Hello
$ ls /tmp/file.txt
ls: cannot access '/tmp/file.txt': No such file or directory
```

Cross-device move:
```bash
$ ./mv /tmp/file.txt /home/user/file.txt
(file is copied to destination, then removed from source)
```

Error case:
```bash
$ ./mv nonexistent.txt /tmp/out.txt
mv: No such file or directory
```

---

## System Calls & Library Routines Used

| Utility | Key calls |
|---------|-----------|
| `pwd`   | `getcwd()`, `free()` |
| `echo`  | `printf()` |
| `cp`    | `open()`, `fstat()`, `read()`, `write()`, `close()` |
| `mv`    | `rename()`, `stat()`, `open()`, `read()`, `write()`, `close()`, `unlink()` |

---

## Notes

- All error messages are printed to `stderr` using `perror()` or `fprintf(stderr, ...)`
- All system call return values are checked
- `mv` handles cross-device moves transparently via a copy-then-delete fallback (triggered when `rename()` returns `EXDEV`)
- `cp` and `mv` preserve source file permissions on the destination
