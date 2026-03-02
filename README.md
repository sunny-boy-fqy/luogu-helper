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

```bash
# Clone the repository
git clone https://github.com/sunny-boy-fqy/luogu-helper.git
cd luogu-helper

# Make scripts executable (Linux/macOS)
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

### 6. Data Generator: `./gen`

Generate custom test data:
```bash
cd P5736
./gen > test.in    # Generate to file
./gen | ./main     # Pipe to your program
```

Customize `gen.cpp` to generate problem-specific data. See `std/gen.cpp` for examples.

### 7. Sync with Google Drive: `./clear` & `./sync`

```bash
# Upload core files to Google Drive and clean up local problem dirs
./clear

# Download core files from Google Drive
./sync
```

**Note:** Requires `rclone` configured with a `gdrive` remote.

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

## Directory Structure

```
luogu-helper/
├── clear              # Upload to Google Drive & cleanup
├── sync               # Download from Google Drive
├── new                # Create new problem
├── std/
│   ├── main.cpp       # Default solution template
│   └── gen.cpp        # Default data generator template
├── include/
│   ├── datagen.hpp    # Data generation library
│   ├── qio.hpp        # Fast IO utilities
│   └── ...            # Other algorithm templates
├── tools/
│   ├── judge          # Local judge
│   ├── checker        # Stress test (compare with std)
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
