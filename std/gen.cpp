/**
 * gen.cpp - Test Data Generator Template
 * 
 * This file generates random test cases for competitive programming problems.
 * Copy this file to your problem directory and customize it.
 * 
 * Usage:
 *   ./gen > data.in    # Generate one test case to file
 *   ./gen              # Generate to stdout (for checker)
 */

#include <bits/stdc++.h>
#include "../include/datagen.hpp"
using namespace std;
using namespace datagen;

int main() {
    // Initialize random seed
    // Use init_rand(seed) for reproducible tests, init_rand() for random
    init_rand();
    
    // ============================================
    // Example: Generate a simple test case
    // Modify this section according to your problem
    // ============================================
    
    // Generate test case parameters
    int n = random_int(5, 20);        // Array size: 5 to 20
    int q = random_int(1, 10);        // Query count: 1 to 10
    
    cout << n << " " << q << "\n";
    
    // Generate a random array
    auto arr = random_array(n, 1, 100);
    print_array(arr);
    
    // Generate random queries
    for (int i = 0; i < q; i++) {
        int l = random_int(1, n);
        int r = random_int(l, n);     // Ensure l <= r
        cout << l << " " << r << "\n";
    }
    
    return 0;
}

/* ============================================
 * COMMON PATTERNS - Uncomment and modify as needed:
 * ============================================

// ----- Pattern 1: Random Array with Constraints -----
// int n = random_int(1, 100000);
// cout << n << "\n";
// auto arr = random_array(n, -1000000000, 1000000000);
// print_array(arr);

// ----- Pattern 2: Random Tree -----
// int n = random_int(2, 100000);
// auto edges = random_tree(n);
// print_tree(n, edges);

// ----- Pattern 3: Random Connected Graph -----
// int n = random_int(2, 1000);
// int m = random_int(n-1, min(10000LL, 1LL*n*(n-1)/2));
// auto edges = random_connected_graph(n, m);
// print_graph(n, edges);

// ----- Pattern 4: Random String -----
// int len = random_int(1, 100000);
// string s = random_string(len, LOWERCASE);
// cout << s << "\n";

// ----- Pattern 5: Random Points -----
// int n = random_int(3, 1000);
// auto pts = random_points(n, -1000, 1000, -1000, 1000);
// cout << n << "\n";
// print_points(pts);

// ----- Pattern 6: Strictly Increasing Sequence -----
// int n = random_int(1, 100000);
// auto seq = random_increasing(n, 1, 1000000000);
// print_array(seq);

// ----- Pattern 7: Random Permutation -----
// int n = random_int(1, 100000);
// auto perm = random_permutation(n);
// print_array(perm);

// ----- Pattern 8: Multiple Test Cases -----
// int T = random_int(1, 10);
// cout << T << "\n";
// while (T--) {
//     int n = random_int(1, 100);
//     cout << n << "\n";
//     auto arr = random_array(n, 1, 100);
//     print_array(arr);
// }

// ----- Pattern 9: Bipartite Graph -----
// int n1 = random_int(1, 100);
// int n2 = random_int(1, 100);
// int m = random_int(1, n1 * n2);
// auto edges = random_bipartite(n1, n2, m);
// cout << n1 << " " << n2 << " " << m << "\n";
// for (auto& e : edges) {
//     cout << e.u << " " << e.v << "\n";
// }

// ----- Pattern 10: Random Matrix -----
// int n = random_int(1, 100);
// int m = random_int(1, 100);
// auto mat = random_matrix(n, m, 0, 1);  // Binary matrix
// cout << n << " " << m << "\n";
// print_matrix(mat);

// ----- Pattern 11: Sequence from Function -----
// auto seq = sequence_from_function(100, [](int i) {
//     return sin(i * 0.1) * 100;
// }, true);  // With noise
// print_array(seq);

// ----- Pattern 12: Weighted Tree -----
// int n = random_int(2, 100000);
// auto edges = random_tree(n, true, 1, 1000000000);
// cout << n << "\n";
// for (auto& e : edges) {
//     cout << e.u << " " << e.v << " " << e.w << "\n";
// }

// ----- Pattern 13: Convex Polygon -----
// int n = random_int(3, 100);
// auto poly = random_convex_polygon(n, -1000, 1000, -1000, 1000);
// print_polygon(poly);

// ----- Pattern 14: Random DAG -----
// int n = random_int(2, 500);
// int m = random_int(1, min(10000LL, 1LL*n*(n-1)/2));
// auto edges = random_dag(n, m);
// print_graph(n, edges);

// ----- Pattern 15: Custom Data Structure -----
// struct TestCase {
//     int n, m;
//     vector<pair<int,int>> pairs;
// };
// TestCase gen() {
//     TestCase tc;
//     tc.n = random_int(1, 100);
//     tc.m = random_int(1, 100);
//     for (int i = 0; i < tc.m; i++) {
//         tc.pairs.push_back({random_int(1, tc.n), random_int(1, tc.n)});
//     }
//     return tc;
// }

*/