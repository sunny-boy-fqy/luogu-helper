#ifndef MAT_HPP
#define MAT_HPP

#include <exception>
#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <type_traits>

#define rep(n,m) for(int i=0;i<n;i++)for(int j=0;j<m;j++)

// --- 整数取模矩阵 ---
template<int N, int mod = 1000000007>
struct mat_int {
    int n, m;
    int a[N][N];

    mat_int(int _n = 0, int _m = 0) : n(_n), m(_m) {
        memset(a, 0, sizeof(a));
    }

    static int pw(int a, int b) {
        int res = 1; a %= mod;
        while (b) {
            if (b & 1) res = 1ll * res * a % mod;
            a = 1ll * a * a % mod;
            b >>= 1;
        }
        return res;
    }

    static int inv(int x) { return pw(x, mod - 2); }

    static mat_int<N, mod> I(int n) {
        mat_int<N, mod> res(n, n);
        for (int i = 0; i < n; i++) res.a[i][i] = 1;
        return res;
    }

    int* operator[](int i) { return a[i]; }
    const int* operator[](int i) const { return a[i]; }

    mat_int operator+(const mat_int& b) const {
        mat_int res(n, m);
        rep(n, m) res.a[i][j] = (a[i][j] + b.a[i][j]) % mod;
        return res;
    }

    mat_int operator-(const mat_int& b) const {
        mat_int res(n, m);
        rep(n, m) res.a[i][j] = (a[i][j] - b.a[i][j] + mod) % mod;
        return res;
    }

    mat_int operator*(const mat_int& b) const {
        mat_int res(n, b.m);
        for (int i = 0; i < n; i++)
            for (int k = 0; k < m; k++)
                for (int j = 0; j < b.m; j++)
                    res.a[i][j] = (res.a[i][j] + 1ll * a[i][k] * b.a[k][j]) % mod;
        return res;
    }

    mat_int operator^(long long b) const {
        mat_int res = I(n), p = *this;
        while (b) {
            if (b & 1) res = res * p;
            p = p * p;
            b >>= 1;
        }
        return res;
    }

    bool operator==(const mat_int& b) const {
        if (n != b.n || m != b.m) return false;
        rep(n, m) if (a[i][j] != b.a[i][j]) return false;
        return true;
    }
};

// --- 浮点数矩阵 ---
template<int N>
struct mat_double {
    int n, m;
    double a[N][N];
    static constexpr double eps = 1e-9;

    mat_double(int _n = 0, int _m = 0) : n(_n), m(_m) {
        memset(a, 0, sizeof(a));
    }

    static mat_double<N> I(int n) {
        mat_double<N> res(n, n);
        for (int i = 0; i < n; i++) res.a[i][i] = 1.0;
        return res;
    }

    double* operator[](int i) { return a[i]; }
    const double* operator[](int i) const { return a[i]; }

    mat_double operator+(const mat_double& b) const {
        mat_double res(n, m);
        rep(n, m) res.a[i][j] = a[i][j] + b.a[i][j];
        return res;
    }

    mat_double operator-(const mat_double& b) const {
        mat_double res(n, m);
        rep(n, m) res.a[i][j] = a[i][j] - b.a[i][j];
        return res;
    }

    mat_double operator*(const mat_double& b) const {
        mat_double res(n, b.m);
        for (int i = 0; i < n; i++)
            for (int k = 0; k < m; k++)
                for (int j = 0; j < b.m; j++)
                    res.a[i][j] += a[i][k] * b.a[k][j];
        return res;
    }

    bool operator==(const mat_double& b) const {
        if (n != b.n || m != b.m) return false;
        rep(n, m) if (std::abs(a[i][j] - b.a[i][j]) > eps) return false;
        return true;
    }
};

// --- 通用矩阵包装器 ---
// 如果 mod == -1，使用 mat_double，否则使用 mat_int
template<int N, int mod = -1>
struct mat : public std::conditional_t<mod == -1, mat_double<N>, mat_int<N, (mod == -1 ? 0 : mod)>> {
    using Base = std::conditional_t<mod == -1, mat_double<N>, mat_int<N, (mod == -1 ? 0 : mod)>>;
    using Base::Base; // 继承构造函数
    
    // 强制转换辅助
    Base& base() { return *this; }
};
// 输出重载
template<int N, int mod>
std::ostream& operator<<(std::ostream& out, const mat<N, mod>& a) {
    for (int i = 0; i < a.n; i++) {
        for (int j = 0; j < a.m; j++) {
            out << a.a[i][j] << (j == a.m - 1 ? "" : " ");
        }
        out << '\n';
    }
    return out;
}

#endif