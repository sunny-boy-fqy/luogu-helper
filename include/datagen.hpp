/**
 * @file datagen.hpp
 * @brief Data Generation Library for Competitive Programming Testing
 * 
 * This header provides utilities for generating random test data including:
 * - Random numbers, arrays, and sequences
 * - Graphs (general graphs, trees, DAGs)
 * - Geometric shapes (polygons, points)
 * - Strings and text (words, sentences, paragraphs)
 * - Vectors and matrices
 * 
 * Usage:
 *   #include "datagen.hpp"
 *   using namespace datagen;
 *   
 *   int main() {
 *       init_rand();  // Initialize random seed
 *       
 *       // Generate a random array
 *       auto arr = random_array(10, 1, 100);
 *       
 *       // Generate a random tree
 *       auto tree = random_tree(10);
 *       
 *       return 0;
 *   }
 */

#ifndef DATAGEN_HPP
#define DATAGEN_HPP

#include <bits/stdc++.h>
using namespace std;

namespace datagen {

constexpr double MATH_PI = 3.14159265358979323846;

// Initialize random seed
inline void init_rand(int seed = -1) {
    if (seed == -1) {
        srand(chrono::steady_clock::now().time_since_epoch().count());
    } else {
        srand(seed);
    }
}

// ==================== Basic Random Numbers ====================

/**
 * Generate a random integer in range [l, r]
 */
inline long long random_int(long long l, long long r) {
    if (l > r) swap(l, r);
    return l + rand() % (r - l + 1);
}

/**
 * Generate a random double in range [l, r]
 */
inline double random_double(double l, double r, int precision = 6) {
    if (l > r) swap(l, r);
    double val = l + (double)rand() / RAND_MAX * (r - l);
    // Round to precision
    double mul = pow(10, precision);
    return round(val * mul) / mul;
}

/**
 * Generate a random boolean with given probability
 */
inline bool random_bool(double prob = 0.5) {
    return (double)rand() / RAND_MAX < prob;
}

// ==================== Array and Sequence Generation ====================

/**
 * Generate a random array of n integers in range [l, r]
 */
inline vector<long long> random_array(int n, long long l, long long r) {
    vector<long long> res(n);
    for (int i = 0; i < n; i++) {
        res[i] = random_int(l, r);
    }
    return res;
}

/**
 * Generate a random permutation of 1..n
 */
inline vector<int> random_permutation(int n) {
    vector<int> res(n);
    iota(res.begin(), res.end(), 1);
    random_shuffle(res.begin(), res.end());
    return res;
}

/**
 * Generate a strictly increasing sequence of n integers in range [l, r]
 */
inline vector<long long> random_increasing(int n, long long l, long long r) {
    if (r - l + 1 < n) {
        throw runtime_error("Range too small for increasing sequence");
    }
    vector<long long> pool;
    for (long long i = l; i <= r && (int)pool.size() < n * 2; i++) {
        pool.push_back(i);
    }
    random_shuffle(pool.begin(), pool.end());
    pool.resize(n);
    sort(pool.begin(), pool.end());
    return pool;
}

/**
 * Generate a strictly decreasing sequence
 */
inline vector<long long> random_decreasing(int n, long long l, long long r) {
    auto res = random_increasing(n, l, r);
    reverse(res.begin(), res.end());
    return res;
}

/**
 * Generate a non-decreasing sequence
 */
inline vector<long long> random_non_decreasing(int n, long long l, long long r) {
    vector<long long> res(n);
    long long cur = l;
    for (int i = 0; i < n; i++) {
        cur = random_int(cur, r);
        res[i] = cur;
    }
    return res;
}

/**
 * Generate a sequence based on mathematical function
 * @param n: sequence length
 * @param func: function f(i) that generates the i-th element (0-indexed)
 * @param add_noise: if true, add small random noise to each element
 */
inline vector<double> sequence_from_function(int n, function<double(int)> func, bool add_noise = false) {
    vector<double> res(n);
    for (int i = 0; i < n; i++) {
        res[i] = func(i);
        if (add_noise) {
            res[i] += random_double(-0.1, 0.1);
        }
    }
    return res;
}

// ==================== String Generation ====================

namespace {
    const string LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
    const string UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const string DIGITS = "0123456789";
    const string ALPHANUMERIC = LOWERCASE + UPPERCASE + DIGITS;
}

/**
 * Generate a random string of length n from given charset
 */
inline string random_string(int n, const string& charset = LOWERCASE) {
    string res;
    res.reserve(n);
    for (int i = 0; i < n; i++) {
        res += charset[rand() % charset.size()];
    }
    return res;
}

/**
 * Generate a random word (length varies)
 */
inline string random_word(int min_len = 2, int max_len = 10, const string& charset = LOWERCASE) {
    return random_string(random_int(min_len, max_len), charset);
}

/**
 * Common English words dictionary (sample)
 */
inline vector<string> common_words() {
    return {
        "the", "be", "to", "of", "and", "a", "in", "that", "have", "i",
        "it", "for", "not", "on", "with", "he", "as", "you", "do", "at",
        "this", "but", "his", "by", "from", "they", "we", "say", "her", "she",
        "or", "an", "will", "my", "one", "all", "would", "there", "their", "what",
        "so", "up", "out", "if", "about", "who", "get", "which", "go", "me",
        "when", "make", "can", "like", "time", "no", "just", "him", "know", "take",
        "people", "into", "year", "your", "good", "some", "could", "them", "see", "other",
        "than", "then", "now", "look", "only", "come", "its", "over", "think", "also"
    };
}

/**
 * Generate a random sentence with n words
 */
inline string random_sentence(int n_words, bool capitalize = true) {
    auto words = common_words();
    string res;
    for (int i = 0; i < n_words; i++) {
        if (i > 0) res += " ";
        res += words[rand() % words.size()];
    }
    if (capitalize && !res.empty()) {
        res[0] = toupper(res[0]);
        res += ".";
    }
    return res;
}

/**
 * Generate a random paragraph with n sentences
 */
inline string random_paragraph(int n_sentences, int min_words = 5, int max_words = 15) {
    string res;
    for (int i = 0; i < n_sentences; i++) {
        if (i > 0) res += " ";
        int words = random_int(min_words, max_words);
        res += random_sentence(words);
    }
    return res;
}

/**
 * Generate a random palindrome string
 */
inline string random_palindrome(int n, const string& charset = LOWERCASE) {
    string half = random_string((n + 1) / 2, charset);
    string rev = half;
    if (n % 2 == 1) rev.pop_back();
    reverse(rev.begin(), rev.end());
    return half + rev;
}

// ==================== Graph Generation ====================

struct Edge {
    int u, v;
    long long w;
    Edge(int u = 0, int v = 0, long long w = 1) : u(u), v(v), w(w) {}
};

/**
 * Generate a random tree with n nodes
 * @param n: number of nodes (1-indexed)
 * @param weighted: whether edges have weights
 * @param w_l, w_r: weight range (if weighted)
 * @return: list of edges
 */
inline vector<Edge> random_tree(int n, bool weighted = false, long long w_l = 1, long long w_r = 100) {
    if (n <= 1) return {};
    vector<Edge> edges;
    edges.reserve(n - 1);
    
    // Use random permutation to create random tree structure
    vector<int> perm = random_permutation(n);
    for (int i = 1; i < n; i++) {
        int parent = perm[random_int(0, i - 1)];
        int child = perm[i];
        long long w = weighted ? random_int(w_l, w_r) : 1;
        edges.emplace_back(parent, child, w);
    }
    
    // Shuffle edges
    random_shuffle(edges.begin(), edges.end());
    return edges;
}

/**
 * Generate a random connected graph with n nodes and m edges
 */
inline vector<Edge> random_connected_graph(int n, int m, bool weighted = false, 
                                            long long w_l = 1, long long w_r = 100,
                                            bool directed = false) {
    if (m < n - 1) {
        throw runtime_error("Too few edges for connected graph");
    }
    if (m > (long long)n * (n - 1) / 2 && !directed) {
        throw runtime_error("Too many edges for undirected simple graph");
    }
    
    set<pair<int,int>> edge_set;
    vector<Edge> edges;
    
    // First create a spanning tree to ensure connectivity
    auto tree = random_tree(n, weighted, w_l, w_r);
    for (auto& e : tree) {
        edge_set.insert({min(e.u, e.v), max(e.u, e.v)});
        edges.push_back(e);
    }
    
    // Add remaining random edges
    while ((int)edges.size() < m) {
        int u = random_int(1, n);
        int v = random_int(1, n);
        if (u == v) continue;
        
        pair<int,int> key = directed ? make_pair(u, v) : make_pair(min(u, v), max(u, v));
        if (edge_set.count(key)) continue;
        
        edge_set.insert(key);
        long long w = weighted ? random_int(w_l, w_r) : 1;
        edges.emplace_back(u, v, w);
    }
    
    random_shuffle(edges.begin(), edges.end());
    return edges;
}

/**
 * Generate a random DAG (Directed Acyclic Graph)
 */
inline vector<Edge> random_dag(int n, int m, bool weighted = false,
                               long long w_l = 1, long long w_r = 100) {
    if (m > (long long)n * (n - 1) / 2) {
        throw runtime_error("Too many edges for DAG");
    }
    
    vector<int> topo = random_permutation(n);
    set<pair<int,int>> edge_set;
    vector<Edge> edges;
    
    while ((int)edges.size() < m) {
        int i = random_int(0, n - 2);
        int j = random_int(i + 1, n - 1);
        int u = topo[i];
        int v = topo[j];
        
        if (edge_set.count({u, v})) continue;
        
        edge_set.insert({u, v});
        long long w = weighted ? random_int(w_l, w_r) : 1;
        edges.emplace_back(u, v, w);
    }
    
    random_shuffle(edges.begin(), edges.end());
    return edges;
}

/**
 * Generate a bipartite graph
 */
inline vector<Edge> random_bipartite(int n1, int n2, int m, bool weighted = false,
                                      long long w_l = 1, long long w_r = 100) {
    if (m > (long long)n1 * n2) {
        throw runtime_error("Too many edges for bipartite graph");
    }
    
    set<pair<int,int>> edge_set;
    vector<Edge> edges;
    
    while ((int)edges.size() < m) {
        int u = random_int(1, n1);
        int v = random_int(n1 + 1, n1 + n2);
        
        if (edge_set.count({u, v})) continue;
        
        edge_set.insert({u, v});
        long long w = weighted ? random_int(w_l, w_r) : 1;
        edges.emplace_back(u, v, w);
    }
    
    random_shuffle(edges.begin(), edges.end());
    return edges;
}

// ==================== Vector Generation ====================

/**
 * Generate n random k-dimensional vectors
 * @param n: number of vectors
 * @param k: dimension
 * @param l, r: coordinate range
 * @param unique: if true, all vectors are distinct
 */
inline vector<vector<long long>> random_vectors(int n, int k, long long l, long long r, bool unique = false) {
    if (unique && pow(r - l + 1, k) < n) {
        throw runtime_error("Range too small for unique vectors");
    }
    
    set<vector<long long>> seen;
    vector<vector<long long>> res;
    res.reserve(n);
    
    while ((int)res.size() < n) {
        vector<long long> vec(k);
        for (int i = 0; i < k; i++) {
            vec[i] = random_int(l, r);
        }
        
        if (unique) {
            if (seen.count(vec)) continue;
            seen.insert(vec);
        }
        
        res.push_back(vec);
    }
    
    return res;
}

/**
 * Generate a random matrix
 */
inline vector<vector<long long>> random_matrix(int n, int m, long long l, long long r) {
    vector<vector<long long>> res(n, vector<long long>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            res[i][j] = random_int(l, r);
        }
    }
    return res;
}

// ==================== Geometry Generation ====================

struct Point {
    long long x, y;
    Point(long long x = 0, long long y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    bool operator<(const Point& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
};

/**
 * Generate n random distinct points
 */
inline vector<Point> random_points(int n, long long x_l, long long x_r, 
                                    long long y_l, long long y_r, bool unique = true) {
    if (unique) {
        set<Point> seen;
        vector<Point> res;
        while ((int)res.size() < n) {
            Point p(random_int(x_l, x_r), random_int(y_l, y_r));
            if (seen.count(p)) continue;
            seen.insert(p);
            res.push_back(p);
        }
        return res;
    } else {
        vector<Point> res;
        res.reserve(n);
        for (int i = 0; i < n; i++) {
            res.emplace_back(random_int(x_l, x_r), random_int(y_l, y_r));
        }
        return res;
    }
}

/**
 * Calculate cross product of vectors (b-a) and (c-a)
 */
inline long long cross(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

/**
 * Calculate squared distance between two points
 */
inline long long dist2(const Point& a, const Point& b) {
    long long dx = a.x - b.x;
    long long dy = a.y - b.y;
    return dx * dx + dy * dy;
}

/**
 * Generate a random convex polygon with n vertices
 */
inline vector<Point> random_convex_polygon(int n, long long x_l, long long x_r,
                                            long long y_l, long long y_r) {
    if (n < 3) {
        throw runtime_error("Convex polygon needs at least 3 vertices");
    }
    
    // Generate points on a circle and add small random perturbations
    vector<Point> pts;
    pts.reserve(n);
    
    double cx = (x_l + x_r) / 2.0;
    double cy = (y_l + y_r) / 2.0;
    double radius = min(x_r - x_l, y_r - y_l) / 3.0;
    
    vector<double> angles;
    for (int i = 0; i < n; i++) {
        angles.push_back(2 * MATH_PI * i / n + random_double(0, 0.5));
    }
    sort(angles.begin(), angles.end());
    
    for (double angle : angles) {
        double r = radius * (0.8 + 0.4 * (double)rand() / RAND_MAX);
        long long x = round(cx + r * cos(angle));
        long long y = round(cy + r * sin(angle));
        x = max(x_l, min(x_r, x));
        y = max(y_l, min(y_r, y));
        pts.emplace_back(x, y);
    }
    
    return pts;
}

/**
 * Generate a random simple polygon (not self-intersecting)
 */
inline vector<Point> random_simple_polygon(int n, long long x_l, long long x_r,
                                            long long y_l, long long y_r) {
    // Simple approach: sort by angle from center
    auto pts = random_points(n, x_l, x_r, y_l, y_r, true);
    
    Point center(0, 0);
    for (auto& p : pts) {
        center.x += p.x;
        center.y += p.y;
    }
    center.x /= n;
    center.y /= n;
    
    sort(pts.begin(), pts.end(), [&](const Point& a, const Point& b) {
        double angle_a = atan2(a.y - center.y, a.x - center.x);
        double angle_b = atan2(b.y - center.y, b.x - center.x);
        return angle_a < angle_b;
    });
    
    return pts;
}

/**
 * Calculate polygon area (shoelace formula)
 * Returns twice the signed area. Positive for counter-clockwise.
 */
inline long long polygon_area2(const vector<Point>& poly) {
    int n = poly.size();
    if (n < 3) return 0;
    
    long long area = 0;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += poly[i].x * poly[j].y - poly[j].x * poly[i].y;
    }
    return abs(area);
}

/**
 * Calculate polygon perimeter
 */
inline double polygon_perimeter(const vector<Point>& poly) {
    int n = poly.size();
    if (n < 2) return 0;
    
    double perim = 0;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        perim += sqrt(dist2(poly[i], poly[j]));
    }
    return perim;
}

// ==================== Output Helpers ====================

/**
 * Print an array separated by spaces
 */
template<typename T>
inline void print_array(const vector<T>& arr, const string& sep = " ", bool newline = true) {
    for (size_t i = 0; i < arr.size(); i++) {
        if (i > 0) cout << sep;
        cout << arr[i];
    }
    if (newline) cout << "\n";
}

/**
 * Print a matrix
 */
template<typename T>
inline void print_matrix(const vector<vector<T>>& mat) {
    for (const auto& row : mat) {
        print_array(row);
    }
}

/**
 * Print a graph in edge list format
 * Format: n m
 *         u v [w]
 */
inline void print_graph(int n, const vector<Edge>& edges, bool print_weight = false) {
    cout << n << " " << edges.size() << "\n";
    for (const auto& e : edges) {
        if (print_weight) {
            cout << e.u << " " << e.v << " " << e.w << "\n";
        } else {
            cout << e.u << " " << e.v << "\n";
        }
    }
}

/**
 * Print a tree (same as graph but ensures n-1 edges)
 */
inline void print_tree(int n, const vector<Edge>& edges, bool print_weight = false) {
    print_graph(n, edges, print_weight);
}

/**
 * Print points
 */
inline void print_points(const vector<Point>& pts) {
    for (const auto& p : pts) {
        cout << p.x << " " << p.y << "\n";
    }
}

/**
 * Print polygon with area information comment
 */
inline void print_polygon(const vector<Point>& poly, bool print_stats = true) {
    if (print_stats) {
        // cerr << "# Polygon: " << poly.size() << " vertices\n";
        // cerr << "# Area (x2): " << polygon_area2(poly) << "\n";
        // cerr << "# Perimeter: " << fixed << setprecision(2) << polygon_perimeter(poly) << "\n";
    }
    cout << poly.size() << "\n";
    print_points(poly);
}

} // namespace datagen

#endif // DATAGEN_HPP
