#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
// 定义精度常量
const double EPS = 1e-10;
const double PI = std::acos(-1.0);

// 符号函数，用于处理精度误差
inline int dcmp(double x) {
    if (std::abs(x) < EPS) return 0;
    return x < 0 ? -1 : 1;
}

template<typename T>
struct Cvector {
    T x, y;
    Cvector() : x(0), y(0) {}
    Cvector(T _x, T _y) : x(_x), y(_y) {}

    friend Cvector operator+(const Cvector& a, const Cvector& b) {
        return Cvector(a.x + b.x, a.y + b.y);
    }
    friend Cvector operator-(const Cvector& a, const Cvector& b) {
        return Cvector(a.x - b.x, a.y - b.y);
    }
    friend Cvector operator*(const Cvector& a, const T& b) {
        return Cvector(a.x * b, a.y * b);
    }
    friend Cvector operator/(const Cvector& a, const T& b) {
        return Cvector(a.x / b, a.y / b);
    }
    
    // 叉积 
    T operator^(const Cvector& b) const {
        return x * b.y - y * b.x;
    }
    // 点积
    T operator*(const Cvector& b) const {
        return x * b.x + y * b.y;
    }

    // 模长的平方
    double len2() const {
        return x * x + y * y;
    }
    // 模长
    double len() const {
        return std::sqrt(len2());
    }
    // 单位向量
    Cvector unit() const {
        double l = len();
        if (dcmp(l) == 0) return *this; // 防止除0
        return Cvector(x / l, y / l);
    }
    // 逆时针旋转90度
    Cvector rot90() const {
        return Cvector(-y, x);
    }
    // 旋转任意角度
    Cvector rot(double ang) const {
        return Cvector(x * std::cos(ang) - y * std::sin(ang), x * std::sin(ang) + y * std::cos(ang));
    }

    // 比较运算符，使用 dcmp 处理精度
    friend bool operator==(const Cvector& a, const Cvector& b) {
        return dcmp(a.x - b.x) == 0 && dcmp(a.y - b.y) == 0;
    }
    friend bool operator!=(const Cvector& a, const Cvector& b) {
        return !(a == b);
    }

    // 输入输出
    void scan(){
        std::cin>>x>>y;
        return ;
    }
    void print(){
        std::cout<<"("<<x<<","<<y<<")";
        return ;
    }
};
#define Cpoint Cvector
template<typename T>
Cvector<T> operator-(const Cvector<T> &a){
    return Cvector<T>(-a.x,-a.y);
}
template<typename T>
double cross(const Cvector<T>& a, const Cvector<T>& b) {
    return a ^ b;
}
template<typename T>
Cvector<T> vec(Cpoint<T> a, Cpoint<T> b) {
    return b-a;
}
// 计算两点间距离
template<typename T>
double dist(const Cvector<T>& a, const Cvector<T>& b) {
    return (a-b).len();
}
template<typename T>
double dist(const Cvector<T>& p, const Cvector<T>& a, const Cvector<T>& b) {
    // 计算点 p 到线段 a-b 的距离
    Cvector<T> ab = b - a;
    Cvector<T> ap = p - a;
    Cvector<T> bp = p - b;

    if (dcmp(ab * ap) < 0) {
        return ap.len();
    } 
    else if (dcmp((-ab) * bp) < 0) {
        return bp.len();
    } 
    else {
        return std::abs(ab ^ ap) / ab.len();
    }
}
template<typename T>
double dist(const Cvector<T>&p,const line<T>& l){
    return dist(p,l.p1,l.p2);
}
//计算两向量夹角
template<typename T>
double angle(const Cvector<T>& a, const Cvector<T>& b) {
    return std::atan2(a ^ b, a * b);
}

//点 p 是否在线段 a-b 上
template<typename T>
bool on_segment(const Cvector<T>& p, const Cvector<T>& a, const Cvector<T>& b) {
    // 叉积为0(共线) 且 点积<=0(在两端点之间)
    return dcmp(vec(a, p) ^ vec(b, p)) == 0 && dcmp(vec(a, p) * vec(b, p)) <= 0;
}
template<typename T>
struct line{
    Cpoint<T> p1,p2;
    line() {}
    line(const Cpoint<T>& _p1, const Cpoint<T>& _p2) : p1(_p1), p2(_p2) {}
    double len(){
        return dist(p1,p2);
    }
};
template<typename T>
struct polygon {
    std::vector<Cpoint<T>> p;

    polygon() {}
    polygon(const std::vector<Cpoint<T>>& _p) : p(_p) {}

    void add_point(const Cpoint<T>& a) {
        p.push_back(a);
    }
    void read_point() {
        Cpoint<T> a;
        a.scan();
        p.push_back(a);
    }
    void read_points(int n) {
        for (int i = 0; i < n; i++) {
            read_point();
        }
        return ;
    }
    void print() const {
        for (Cpoint<T> point : p) {
            point.print();
            std::cout << " ";
        }
        std::cout << std::endl;
    }
    void clear() {
        p.clear();
    }

    double area() const {
        double res = 0;
        int n = p.size();
        if (n < 3) return 0;
        for (int i = 0; i < n; i++) {
            res += p[i] ^ p[(i + 1) % n];
        }
        return std::abs(res) / 2.0;
    }
    // 判断点是否是多边形的顶点
    bool is_on_point(const Cpoint<T>& a) const {
        return std::find(p.begin(), p.end(), a) != p.end();
    }
    // 判断点是否在多边形边上
    bool is_on_boundary(const Cpoint<T>& a) const {
        int n = p.size();
        for (int i = 0; i < n; i++) {
            if (on_segment(a, p[i], p[(i + 1) % n])) {
                return true;
            }
        }
        return false;
    }

    // 判断点是否在多边形内部
    // 0: 外部, 1: 内部, -1: 边界上
    int is_in(const Cpoint<T>& a) const {
        if (is_on_boundary(a)) return true; 

        int wn = 0;
        int n = p.size();
        for (int i = 0; i < n; i++) {
            const Cpoint<T>& p1 = p[i];
            const Cpoint<T>& p2 = p[(i + 1) % n];
            
            // 叉积判断点在有向线段的左侧还是右侧
            double k = vec(p1, p2) ^ vec(p1, a);

            // 向上穿过射线 (p1在下，p2在上)
            if (dcmp(p1.y - a.y) <= 0 && dcmp(p2.y - a.y) > 0 && dcmp(k) > 0) {
                wn++;
            }
            // 向下穿过射线 (p1在上，p2在下)
            else if (dcmp(p1.y - a.y) > 0 && dcmp(p2.y - a.y) <= 0 && dcmp(k) < 0) {
                wn--;
            }
        }
        return wn;
    }
    bool is_seq(){
        int n=p.size();
        if(n<3)return true;
        int dir=0;
        for(int i=0;i<n;i++){
            int cross=dcmp(vec(p[(i+1)%n],p[(i+2)%n]) ^ vec(p[(i+1)%n],p[i]));
            if(cross!=0){
                if(dir==0)dir=cross;
                else if(dir*cross<0)return false;
            }
        }
        return true;
    }
        polygon convex_hull() const {
        std::vector<Cpoint<T>> pts = p;
        int n = pts.size();
        if (n < 3) return polygon(pts);

        // 1. 排序：优先 x 小，其次 y 小
        std::sort(pts.begin(), pts.end(), [](const Cpoint<T>& a, const Cpoint<T>& b) {
            return a.x < b.x || (a.x == b.x && a.y < b.y);
        });

        std::vector<Cpoint<T>> hull(2 * n);
        int k = 0;

        // 2. 构建下凸包 (Lower Hull)
        for (int i = 0; i < n; i++) {
            // 向量顺序：(倒数第二个点 -> 栈顶点) X (栈顶点 -> 当前点)
            // 如果叉积 <= 0，说明是右拐或共线，不满足凸包性质（我们需要左拐），弹出栈顶
            while (k >= 2 && dcmp(vec(hull[k - 2], hull[k - 1]) ^ vec(hull[k - 1], pts[i])) <= 0) {
                k--;
            }
            hull[k++] = pts[i];
        }

        // 3. 构建上凸包 (Upper Hull)
        // 注意：t = k + 1 是为了保护下凸包的点不被弹出（除了下凸包的终点，它也是上凸包的起点）
        for (int i = n - 2, t = k + 1; i >= 0; i--) {
            // 逻辑完全同上
            while (k >= t && dcmp(vec(hull[k - 2], hull[k - 1]) ^ vec(hull[k - 1], pts[i])) <= 0) {
                k--;
            }
            hull[k++] = pts[i];
        }

        // 4. 调整大小
        // 此时 hull[0] 和 hull[k-1] 是同一个点（起点），通常为了多边形表示不重复存起点，去掉最后一个
        hull.resize(k - 1);
        return polygon(hull);
    }
    double len() const {
        double res=0;
        int n=p.size();
        for(int i=0;i<n;i++){
            res+=dist(p[i],p[(i+1)%n]);
        }
        return res;
    }
};

#endif