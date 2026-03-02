# Luogu Helper

A comprehensive toolkit for competitive programming (OI/ACM) development, optimized for use with **VSCode**.

## Features

- **Problem Setup**: Quickly create new problem directories with standard templates
- **Data Fetching**: Fetch problem statements, samples, and solutions from Luogu
- **Testing**: Built-in judge and checker for local testing
- **Data Generation**: Powerful library for generating random test cases
- **Sync**: Backup and sync your code with Google Drive
- **Cross-platform**: Support for Linux/macOS and Windows

## Quick Start

### Setup

#### Method 1: Clone with Git (Recommended)

```bash
# Clone the repository
git clone https://github.com/sunny-boy-fqy/luogu-helper.git
cd luogu-helper

# Make scripts executable (Linux/macOS)
chmod +x clear new sync tools/*
```

#### Method 2: Download with curl/wget

If you don't have git or want a quick download:

**Using curl:**
```bash
# Create directory
mkdir -p luogu-helper && cd luogu-helper

# Download individual files
curl -O https://raw.githubusercontent.com/sunny-boy-fqy/luogu-helper/main/clear
curl -O https://raw.githubusercontent.com/sunny-boy-fqy/luogu-helper/main/clear.bat
curl -O https://raw.githubusercontent.com/sunny-boy-fqy/luogu-helper/main/new
curl -O https://raw.githubusercontent.com/sunny-boy-fqy/luogu-helper/main/new.bat
curl -O https://raw.githubusercontent.com/sunny-boy-fqy/luogu-helper/main/sync
curl -O https://raw.githubusercontent.com/sunny-boy-fqy/luogu-helper/main/sync.bat
curl -O https://raw.githubusercontent.com/sunny-boy-fqy/luogu-helper/main/README.md

# Download directories as tarballs
curl -L https://api.github.com/repos/sunny-boy-fqy/luogu-helper/tarball/main | tar xz --strip-components=1
```

**Using wget:**
```bash
# Download entire repository as zip
wget https://github.com/sunny-boy-fqy/luogu-helper/archive/refs/heads/main.zip
unzip main.zip
cd luogu-helper-main

# Or download tarball
wget -O - https://api.github.com/repos/sunny-boy-fqy/luogu-helper/tarball/main | tar xz
cd sunny-boy-fqy-luogu-helper-*/
```

**One-liner install (Linux/macOS):**
```bash
# Download and extract in one command
curl -L https://api.github.com/repos/sunny-boy-fqy/luogu-helper/tarball/main | tar xz --strip-components=1 -C luogu-helper

# Or with wget
wget -qO- https://api.github.com/repos/sunny-boy-fqy/luogu-helper/tarball/main | tar xz --strip-components=1 -C luogu-helper
```

**Make scripts executable:**
```bash
chmod +x clear new sync tools/*
```

### Recommended: VSCode Integration

This toolkit works best with VSCode. Recommended extensions:
- C/C++ Extension Pack
- Competitive Programming Helper

Add to `.vscode/settings.json`:
```json
{
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.intelliSenseMode": "linux-gcc-x64"
}
```

## Commands

### 1. Create New Problem: `./new`

```bash
# Create from Luogu problem ID
./new P5736
./new T721517

# Create blank directory
./new my_contest_problem
```

This creates a new directory with:
- `main.cpp` - Your solution template
- `gen.cpp` - Data generator (copy from std/)
- `data/` - Sample test cases (if fetched from Luogu)
- `info` - Problem metadata

### 2. Fetch Problem Info: `./get_problem`

Run inside a problem directory to fetch the full problem statement:
```bash
cd P5736
./get_problem   # Creates problem.md
```

### 3. Fetch Solution: `./get_solve`

Fetches the highest-rated AC solution from Luogu:
```bash
cd P5736
./get_solve     # Creates std.cpp
```

### 4. Local Judge: `./judge`

Test your solution against sample data:
```bash
cd P5736
./judge         # Compiles and tests main.cpp
```

### 5. Checker (Stress Test): `./checker`

Compare your solution with the standard solution using random tests:
```bash
cd P5736

# First, get/copy std.cpp
./get_solve

# Or copy from another source
cp /path/to/solution.cpp std.cpp

# Then run checker
./checker [num_tests] [timeout]
./checker 100 2    # Run 100 tests with 2s timeout
```

**Checker Features:**
- Automatically compiles gen.cpp, main.cpp, and std.cpp
- Generates random test data
- Compares outputs (ignoring whitespace differences)
- Stops at first mismatch and saves failing case to `fail.in`
- Progress report every 10 tests

### 6. Bundle Code: `./bundle`

Bundle your code with all referenced local headers into a single file, making it easy to submit to online judges:
```bash
cd P5736
./bundle          # Bundle main.cpp, automatically expand headers from include/
./bundle my.cpp   # Bundle a specific source file
```

**Bundle Features:**
- Automatically expands `#include "..."` references to local headers (searches in `include/` directory or relative paths)
- Automatically removes `#pragma once` to avoid redefinition issues
- Copies the bundled code to clipboard automatically (supports Linux, macOS, Windows)
- If clipboard access fails, saves to `bundled_code.cpp`

**Example:**
```cpp
// main.cpp
#include "../include/modint.hpp"
#include "../include/qio.hpp"

int main() {
    // Your code...
}
```

After running `./bundle`, the generated code will contain the full content of `modint.hpp` and `qio.hpp`.

### 7. Data Generator: `./gen`

Generate custom test data:
```bash
cd P5736
./gen > test.in    # Generate to file
./gen | ./main     # Pipe to your program
```

Customize `gen.cpp` to generate problem-specific data. See `std/gen.cpp` for examples.

### 7. Sync with Google Drive: `./clear` & `./sync`

These commands allow you to backup your templates and tools to Google Drive, and sync them across multiple devices.

```bash
# Upload core files to Google Drive and clean up local problem dirs
./clear

# Download core files from Google Drive
./sync
```

#### Setting up rclone

**Step 1: Install rclone**

**Linux:**
```bash
# Method 1: Using snap
sudo snap install rclone

# Method 2: Download binary
curl https://rclone.org/install.sh | sudo bash

# Method 3: Using package manager
# Debian/Ubuntu
sudo apt install rclone

# Fedora
sudo dnf install rclone

# Arch
sudo pacman -S rclone
```

**macOS:**
```bash
# Using Homebrew
brew install rclone

# Or download from https://rclone.org/downloads/
```

**Windows:**
1. Download from https://rclone.org/downloads/
2. Extract to a folder (e.g., `C:\rclone`)
3. Add to PATH: Open System Properties → Environment Variables → Edit PATH → Add `C:\rclone`
4. Verify: Open CMD and type `rclone version`

**Step 2: Configure Google Drive**

```bash
# Start configuration
rclone config
```

Follow these steps:
1. Type `n` for new remote
2. Name: `gdrive` (must match what scripts expect)
3. Select `13` for Google Drive (or the corresponding number)
4. Leave client_id and client_secret empty (press Enter)
5. Select scope: `1` (Full access)
6. Root folder ID: leave empty
7. Service Account: leave empty
8. Edit advanced config: `n`
9. Use auto config: `y` (for desktop) or `n` (for headless/SSH)
   - If `y`: Browser will open, log in to Google and authorize
   - If `n`: Follow the URL provided, get code, paste back
10. Confirm: `y`

Verify configuration:
```bash
rclone listremotes          # Should show: gdrive:
rclone lsd gdrive:          # List your Drive root
```

**Step 3: Create sync folder (optional)**

By default, scripts sync to `gdrive:OI/program`. You can create this folder:
```bash
rclone mkdir gdrive:OI/program
```

Or change the path in `clear` and `sync` scripts by editing the `REMOTE` variable.

**Step 4: Test sync**

```bash
# Test upload (dry run)
rclone sync . gdrive:OI/program --dry-run

# Test download (dry run)
rclone sync gdrive:OI/program . --dry-run
```

**What gets synced:**
- `std/` - Templates
- `tools/` - All scripts and utilities
- `include/` - Header libraries
- `.vscode/` - VSCode settings
- `new`, `clear`, `sync` scripts
- All `main.cpp` files in problem directories

**What doesn't get synced:**
- Problem directories (only `main.cpp` inside them)
- Compiled binaries
- Test data outputs
- Personal solution files

#### Troubleshooting

**"gdrive not found" error:**
```bash
# Re-run config
rclone config

# Check if 'gdrive' is in the list
rclone listremotes
```

**Token expired:**
```bash
# Refresh token
rclone config reconnect gdrive:
```

**Rate limiting:**
Google Drive has API limits. If you hit them, wait a few minutes or add `--tpslimit 1` to sync commands.

**Large files:**
For files > 100MB, rclone may need special handling. Use `--drive-chunk-size 128M` flag.

## Data Generation Library

Include `datagen.hpp` for powerful test data generation:

```cpp
#include "../include/datagen.hpp"
using namespace datagen;

int main() {
    init_rand();  // Initialize random seed
    
    // Basic random numbers
    int x = random_int(1, 100);
    double y = random_double(0.0, 1.0);
    
    // Arrays and sequences
    auto arr = random_array(10, 1, 100);           // Random array
    auto perm = random_permutation(10);             // Permutation
    auto inc = random_increasing(10, 1, 100);       // Strictly increasing
    auto seq = sequence_from_function(10, [](int i) { return i * i; });
    
    // Strings
    string s = random_string(10, LOWERCASE);
    string word = random_word(3, 8);
    string sentence = random_sentence(10);
    string para = random_paragraph(5);
    
    // Graphs
    auto tree = random_tree(100);                   // Unweighted tree
    auto wtree = random_tree(100, true, 1, 100);   // Weighted tree
    auto graph = random_connected_graph(100, 200);  // Connected graph
    auto dag = random_dag(50, 100);                 // DAG
    auto bip = random_bipartite(20, 30, 100);       // Bipartite graph
    
    // Vectors and matrices
    auto vecs = random_vectors(10, 3, -100, 100);   // 10 3D vectors
    auto mat = random_matrix(5, 5, 0, 1);           // 5x5 matrix
    
    // Geometry
    auto pts = random_points(10, -100, 100, -100, 100);
    auto poly = random_convex_polygon(10, -100, 100, -100, 100);
    auto simple = random_simple_polygon(10, -100, 100, -100, 100);
    
    // Polygon calculations
    long long area2 = polygon_area2(poly);          // Twice the area
    double perim = polygon_perimeter(poly);         // Perimeter
    
    // Output helpers
    print_array(arr);
    print_graph(100, edges, true);                  // With weights
    print_points(pts);
    print_polygon(poly);
    
    return 0;
}
```

### Library Features

| Category | Functions |
|----------|-----------|
| **Random Numbers** | `random_int()`, `random_double()`, `random_bool()` |
| **Arrays** | `random_array()`, `random_permutation()`, `random_increasing()`, `random_decreasing()`, `random_non_decreasing()`, `sequence_from_function()` |
| **Strings** | `random_string()`, `random_word()`, `random_sentence()`, `random_paragraph()`, `random_palindrome()` |
| **Graphs** | `random_tree()`, `random_connected_graph()`, `random_dag()`, `random_bipartite()` |
| **Vectors** | `random_vectors()`, `random_matrix()` |
| **Geometry** | `random_points()`, `random_convex_polygon()`, `random_simple_polygon()`, `polygon_area2()`, `polygon_perimeter()` |
| **Output** | `print_array()`, `print_matrix()`, `print_graph()`, `print_tree()`, `print_points()`, `print_polygon()` |

## Header Libraries (include/)

The `include/` directory contains a collection of commonly used competitive programming header libraries that can be directly included in your code:

| Header | Description |
|--------|-------------|
| `qio.hpp` | Fast IO utilities, including optimized read/write and memory mapping |
| `modint.hpp` | Modulo integer class template supporting common modular arithmetic operations |
| `datagen.hpp` | Data generation library (see above) |
| `bint.hpp` | Big integer (arbitrary precision) arithmetic |
| `AC_trie.hpp` | Aho-Corasick automaton for multi-pattern string matching |
| `BIT.hpp` | Binary Indexed Tree (Fenwick Tree) |
| `treap.hpp` | Treap (randomized balanced binary search tree) |
| `LCT.hpp` | Link-Cut Tree (dynamic tree data structure) |
| `Dij.hpp` | Dijkstra shortest path algorithm wrapper |
| `mat.hpp` | Matrix operations library |
| `comb.hpp` | Combinatorics utilities, including factorials and modular inverses |
| `geometry.hpp` | Computational geometry tools |
| `fast_sort.hpp` | Fast sorting utilities |
| `math_constants.hpp` | Mathematical constants |
| `ai.hpp` | AI/Machine learning utilities |

**Usage:**
```cpp
#include "../include/modint.hpp"
#include "../include/qio.hpp"

using mint = modint<1000000007>;

int main() {
    qio::fast_input in;
    qio::fast_output out;
    
    int n;
    in >> n;
    
    mint ans = 1;
    for (int i = 1; i <= n; i++) {
        ans *= i;
    }
    out << ans << '\n';
    
    return 0;
}
```

Before submitting, use the `./bundle` command to pack the header contents into your code.

## Directory Structure

```
luogu-helper/
├── clear              # Upload to Google Drive & cleanup
├── sync               # Download from Google Drive
├── new                # Create new problem
├── std/
│   ├── main.cpp       # Default solution template
│   └── gen.cpp        # Default data generator template
├── include/           # Header libraries
│   ├── datagen.hpp    # Data generation library
│   ├── qio.hpp        # Fast IO utilities
│   ├── modint.hpp     # Modulo integer class
│   ├── bint.hpp       # Big integer
│   ├── AC_trie.hpp    # Aho-Corasick automaton
│   ├── BIT.hpp        # Binary Indexed Tree
│   ├── treap.hpp      # Treap
│   ├── LCT.hpp        # Link-Cut Tree
│   ├── Dij.hpp        # Dijkstra algorithm
│   ├── mat.hpp        # Matrix operations
│   ├── comb.hpp       # Combinatorics
│   ├── geometry.hpp   # Computational geometry
│   ├── fast_sort.hpp  # Fast sorting
│   ├── math_constants.hpp  # Math constants
│   └── ai.hpp         # AI utilities
├── tools/
│   ├── judge          # Local judge
│   ├── checker        # Stress test (compare with std)
│   ├── bundle         # Bundle code (expand headers)
│   ├── get_problem    # Fetch problem statement
│   ├── get_solve      # Fetch AC solution
│   └── fetch_problem.py
└── .vscode/           # VSCode settings
```

## Windows Support

For Windows users, use the `.bat` versions of scripts:

```cmd
new.bat P5736
checker.bat 100 2
judge.bat
```

Or use WSL (Windows Subsystem for Linux) for full Linux compatibility.

## Environment Variables

- `LUOGU_COOKIE`: Set your Luogu session cookie for accessing private problems

```bash
export LUOGU_COOKIE="_uid=12345; __client_id=..."
```

Or create `~/.luogu_cookie` file with your cookie.

## Tips

1. **Use VSCode snippets**: Add your commonly used code patterns to VSCode snippets
2. **Customize gen.cpp**: Copy `std/gen.cpp` to your problem directory and modify
3. **Checker for debugging**: Use `./checker` extensively to catch edge cases
4. **Keep std.cpp**: Always have a correct reference solution for comparison
5. **Google Drive backup**: Run `./clear` regularly to backup your progress

## Requirements

- **Linux/macOS**: bash, g++, python3, rclone (for sync)
- **Windows**: MinGW-w64 or MSVC, python3, batch support
- **All platforms**: C++17 compiler, git (optional)

## License

MIT License - Feel free to use and modify for your competitive programming journey!

## Contributing

Contributions are welcome! Please ensure:
- Code follows existing style
- Scripts remain portable (no hardcoded paths)
- Add tests for new features
- Update documentation

---

**Happy Coding!** 🚀
