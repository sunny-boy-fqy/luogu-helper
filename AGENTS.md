# PROJECT KNOWLEDGE BASE

**Generated:** 2026-03-04
**Commit:** 9dfe7e3
**Branch:** main

## OVERVIEW

C++ competitive programming toolkit for 洛谷 (Luogu) OJ with Python tooling. Provides problem setup, local testing, data generation, and Google Drive sync. Cross-platform (Linux/macOS/Windows).

## STRUCTURE

```
luogu-helper/
├── include/     # Header-only algorithm library (14 files)
├── std/         # Code templates (main.cpp, gen.cpp) + symlinks to tools/
├── tools/       # Core scripts (judge, checker, bundle, get_problem, get_solve)
├── .vscode/     # VSCode C++ configuration
└── Root scripts: new, clear, sync, config_cookie (shell + .bat variants)
```

## WHERE TO LOOK

| Task | Location | Notes |
|------|----------|-------|
| Add algorithm template | `include/*.hpp` | Header-only library |
| Modify problem setup | `new`, `tools/fetch_problem.py` | Bash wrapper + Python fetcher |
| Local testing logic | `tools/judge` | Compiles & tests main.cpp |
| Stress testing | `tools/checker` | Compares main.cpp vs std.cpp |
| Code bundling | `tools/bundle.py` | Expands #include "..." |
| Data generation | `include/datagen.hpp` | Random data library |
| Fast I/O | `include/qio.hpp` | mmap-optimized input |
| Cookie config | `config_cookie`, `~/.luogu_cookie` | Luogu API authentication |

## CONVENTIONS

**C++**:
- Standard: C++17
- Compiler: g++ (Linux), MSVC/MinGW (Windows)
- Warning flags: `-Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wconversion -Wsign-conversion -Wnull-dereference`
- Headers: Use `#include "../include/X.hpp"` in problem code, bundle before submit

**Cross-Platform**:
- Each tool has shell script (Linux/macOS) + `.bat` file (Windows)
- Symlinks in `std/` point to `tools/` for running from problem directories
- Windows build creates actual files instead of symlinks

**Problem Workflow**:
1. `./new P1234` → creates `P1234/` with main.cpp, gen.cpp, data/
2. Edit `main.cpp` (solution) and `gen.cpp` (test generator)
3. `./judge` → tests against sample data
4. `./checker 100 2` → stress test with 100 random cases, 2s timeout
5. `./bundle` → expand includes to single file for submission

**Google Drive Sync**:
- Requires `rclone` configured with remote named `gdrive`
- `./clear` → upload templates/tools, clean local problems
- `./sync` → download from Drive
- Target: `gdrive:OI/program`

## ANTI-PATTERNS (THIS PROJECT)

- **DO NOT** mix `scanf/printf` with `qio.hpp` fast I/O
- **DO NOT** forget `./bundle` before submitting to OJ (includes not expanded)
- **DO NOT** commit problem directories (P*/, T*/, U*/ ignored by git)
- **AVOID** hardcoding paths in scripts (use `$(dirname "${BASH_SOURCE[0]}")`)

## UNIQUE STYLES

**Symlink Architecture**:
- `std/` contains symlinks (`judge -> ../tools/judge`) to run tools from problem directories
- Copy `std/*` to problem dir, then `./judge` works inside problem

**Dual Implementation**:
- Some tools exist in 3 forms: shell, Python, and `.bat`
- Example: `bundle` (shell) → `bundle.py` (Python) → `bundle.bat` (Windows)

**Header Library Pattern**:
- `include/datagen.hpp` is both a library AND a template (copied to gen.cpp)
- All headers are standalone, no interdependencies except `qio.hpp` → `bint.hpp`

## COMMANDS

```bash
# Setup
./new P5736                      # Create problem directory
./new my_contest_problem         # Create blank directory

# Testing
./judge                          # Test main.cpp with sample data
./checker 100 2                  # Stress test (100 cases, 2s timeout)

# Code preparation
./bundle                         # Expand includes to clipboard/bundled_code.cpp
./bundle my.cpp                  # Bundle specific file

# Luogu API (requires cookie)
./get_problem                    # Fetch problem description to problem.md
./get_solve                      # Fetch top-rated AC solution to std.cpp

# Google Drive sync (requires rclone)
./clear                          # Upload tools/templates, clean local
./sync                           # Download from Drive
```

## NOTES

- **Luogu Cookie**: Set `LUOGU_COOKIE` env var or create `~/.luogu_cookie` file
- **Cookie format**: `_uid=12345; __client_id=...`
- **Windows**: Use `.bat` scripts or WSL for full compatibility
- **Bundle clipboard**: Requires `xsel` or `xclip` on Linux
- **Timeout command**: Uses `timeout` (Linux) or `gtimeout` (macOS with coreutils)
- **No tests**: No formal test suite - testing is manual via `judge`/`checker`