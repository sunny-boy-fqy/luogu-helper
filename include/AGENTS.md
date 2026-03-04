# INCLUDE LIBRARY

Header-only C++ algorithm library for competitive programming. 14 standalone headers with no interdependencies (except `qio.hpp` → `bint.hpp`).

## STRUCTURE

```
include/
├── qio.hpp          # Fast I/O (mmap, lookup tables)
├── modint.hpp       # Modular arithmetic template
├── datagen.hpp      # Random test data generation
├── bint.hpp         # Big integer arithmetic
├── AC_trie.hpp      # Aho-Corasick automaton
├── BIT.hpp          # Binary Indexed Tree (Fenwick)
├── treap.hpp        # Treap balanced BST
├── LCT.hpp          # Link-Cut Tree
├── Dij.hpp          # Dijkstra shortest path
├── mat.hpp          # Matrix operations
├── comb.hpp         # Combinatorics (factorial, inverse)
├── geometry.hpp     # Computational geometry
├── fast_sort.hpp    # Optimized sorting
└── math_constants.hpp  # Math constants (PI, E, etc.)
```

## WHERE TO LOOK

| Need | Header | Key Features |
|------|--------|--------------|
| Fast input/output | `qio.hpp` | mmap on Linux, 64MB buffer, lookup tables for 2-digit I/O |
| Modular arithmetic | `modint.hpp` | Template `modint<MOD>`, automatic normalization |
| Test data generation | `datagen.hpp` | Random arrays, trees, graphs, strings, geometry |
| Big integers | `bint.hpp` | Arbitrary precision, all operators |
| Pattern matching | `AC_trie.hpp` | Multi-pattern string search |
| Range queries | `BIT.hpp` | Point update, prefix sum |
| Balanced BST | `treap.hpp` | Randomized priority, all BST ops |
| Dynamic trees | `LCT.hpp` | Link-cut operations, path queries |
| Shortest path | `Dij.hpp` | Priority queue Dijkstra wrapper |
| Matrix ops | `mat.hpp` | Multiplication, exponentiation, Gaussian elimination |
| Combinatorics | `comb.hpp` | Precomputed factorial, inverse, nCr |
| Geometry | `geometry.hpp` | Point, line, polygon operations |

## CONVENTIONS

- Use `#include "../include/X.hpp"` in problem code
- Run `./bundle` before submission to expand headers
- All headers standalone except `qio.hpp` → `bint.hpp`

## ANTI-PATTERNS

- **DO NOT** use `qio.hpp` with `scanf/printf` (incompatible)
- **DO NOT** forget `init_rand()` before datagen functions
- **AVOID** modifying headers in place

## UNIQUE STYLES

**qio.hpp**: Platform-specific fast I/O (mmap on Linux, 64MB buffer, lookup tables for 2-digit I/O)

**datagen.hpp**: Returns vectors/structs directly, graph functions use `Edge{u,v,w}`, consistent `random_X()` / `print_X()` naming

**modint.hpp**: Template `modint<MOD>`, auto-normalization, implicit from `int/ll`, explicit to `int/ll/bool`

## NOTES

- `bint.hpp` (73KB), `datagen.hpp` (40+ functions), `qio.hpp` (replaces cin/cout)
- No test suite - battle-tested in competitive programming