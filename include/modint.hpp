#ifndef MODINT_HPP
#define MODINT_HPP

#ifndef ll
#define ll long long
#endif

#include <iostream>
#include <stdexcept>
#include <functional>

template<ll mod>
struct modint {
private:
    ll val;
    
    static ll norm(const ll x) {
        ll res = x % mod;
        return res < 0 ? res + mod : res;
    }
    
public:
    modint() : val(0) {}
    modint(const int m) : val(norm(m)) {}
    modint(const ll m) : val(norm(m)) {}
    
    explicit operator int() const { return val; }
    explicit operator ll() const { return val; }
    explicit operator bool() const { return val != 0; }
    int to_int() const { return val; }
    
    // 修复输入输出运算符
    friend std::istream& operator>>(std::istream& in, modint& x) {
        ll temp;
        in >> temp;
        x.val = norm(temp);
        return in;
    }
    
    friend std::ostream& operator<<(std::ostream& out, const modint& x) {
        out << x.val;
        return out;
    }
    
    // 赋值运算符
    modint& operator=(const int m) {
        val = norm(m);
        return *this;
    }
    
    modint& operator=(const ll m) {
        val = norm(m);
        return *this;
    }
    
    // 算术运算符
    modint operator-() const { return modint(norm(-val)); }
    
    modint operator+(const modint& o) const {
        return modint(norm(val + o.val));
    }
    
    modint operator-(const modint& o) const {
        return modint(norm(val - o.val));
    }
    
    modint operator*(const modint& o) const {
        return modint(norm(1ll * val * o.val % mod));
    }
    
    // 复合赋值运算符
    modint& operator+=(const modint& o) {
        val = norm(val + o.val);
        return *this;
    }
    
    modint& operator-=(const modint& o) {
        val = norm(val - o.val);
        return *this;
    }
    
    modint& operator*=(const modint& o) {
        val = norm(1ll * val * o.val % mod);
        return *this;
    }
    
    // 比较运算符
    bool operator==(const modint& o) const { return val == o.val; }
    bool operator!=(const modint& o) const { return val != o.val; }
    bool operator<(const modint& o) const { return val < o.val; }
    bool operator>(const modint& o) const { return val > o.val; }
    
    // 修复幂运算方法
    static modint pw(modint a, ll b) {
        modint res(1);
        while (b) {
            if (b & 1) res *= a;
            a *= a;
            b >>= 1;
        }
        return res;
    }
    
    // 修复阶乘方法
    static modint fac(int a) {
        modint res(1);
        for (int i = 1; i <= a; i++) {
            res *= i;
        }
        return res;
    }
    
    // 修复求逆方法
    modint inv() const {
        if (std::__gcd(val,mod)!=1) throw std::logic_error("无乘法逆元");
        
        ll a = val, b = mod, u = 1, v = 0;
        while (b) {
            ll t = a / b;
            a -= t * b; std::swap(a, b);
            u -= t * v; std::swap(u, v);
        }
        return modint(u);
    }
    
    // 修复除法运算符
    modint operator/(const modint& o) const {
        return *this * o.inv();
    }
    
    modint& operator/=(const modint& o) {
        return *this = *this / o;
    }
    
    // 取模运算符
    modint operator%(const int o) const {
        return modint(val % o);
    }
    
    modint& operator%=(const int o) {
        val = val % o;
        return *this;
    }
    
    // 支持与整数的混合运算
    friend modint operator+(int lhs, const modint& rhs) {
        return modint(lhs) + rhs;
    }
    
    friend modint operator-(int lhs, const modint& rhs) {
        return modint(lhs) - rhs;
    }
    
    friend modint operator*(int lhs, const modint& rhs) {
        return modint(lhs) * rhs;
    }
    
    friend modint operator/(int lhs, const modint& rhs) {
        return modint(lhs) / rhs;
    }

    // 获取原始值
    ll value() const { return val; }
};

#endif