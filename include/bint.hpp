#ifndef BINT
#define BINT
#define NEED_POW
// #define FFT_MULTI
#ifdef NEED_POW
#include <cmath>
#endif
#include<string>
#include<cstring>
#include<limits.h>
#include<vector>
#include<iostream>
#include<iomanip>
typedef long long LL;

constexpr int N = 160;

#ifdef FFT_MULTI

typedef struct complex
{
  static constexpr double PI = 3.141592653589793238;
  complex(const double _x = 0, const double _y = 0) : x(_x), y(_y) {}
  double x, y;
  inline static complex omega(const int k, const int n)
  {
    return complex(cos(2.0 * k * PI / (double)n), sin(2.0 * k * PI / (double)n));
  }
  inline complex operator+(const complex Q) const
  {
    return complex(x + Q.x, y + Q.y);
  }
  inline complex operator-(const complex Q) const
  {
    return complex(x - Q.x, y - Q.y);
  }
  inline complex operator*(const complex Q) const
  {
    return complex(x * Q.x - y * Q.y, x * Q.y + y * Q.x);
  }
  inline complex conj() const { return complex(x, -y); }
  friend complex conj(const complex A) { return A.conj(); }
  inline complex abs() { return hypot(x, y); }
  friend ostream &operator<<(ostream &os, const complex Q)
  {
    return os << '(' << Q.x << ", " << Q.y << ")", os;
  }
} Comp;
void FFT_base_virt_change(const int n, vector<Comp> &f, const bool inv)
{
  if (n == 1)
    return;
  const int m = n >> 1;

  vector<Comp> f1(m + 1), f2(m + 1);

  for (int i = 0; i != m; i++)
  {
    f1[i] = f[i << 1];
    f2[i] = f[i << 1 | 1];
  }

  FFT_base_virt_change(m, f1, inv);
  FFT_base_virt_change(m, f2, inv);

  const Comp _w_multi = Comp(cos(2.0 * Comp::PI / n), sin((inv ? -1.0 : 1.0) * 2.0 * Comp::PI / n));
  Comp _w0 = Comp(1, 0);

  for (int i = 0; i != m; i++, _w0 = _w0 * _w_multi)
  {
    if (!(i & 32767))
      _w0 = Comp(cos(2.0 * i * Comp::PI / n), sin((inv ? -1.0 : 1.0) * 2.0 * i * Comp::PI / n)); // 这个地方很有可能有误差, 所以需要不时重新计算减小误差

    // 从 1 到 32768 效果是显著的, 然而从 32768 到更大的数, 效果并不明显

    f[i] = f1[i] + _w0 * f2[i];
    f[i + m] = f1[i] - _w0 * f2[i];
  }

  return;
}
#endif
struct bint
{
  bint()
  {
    food = 0;
    num.clear();
  }
#ifdef FFT_MULTI
  vector<long long> num;
#else
  std::vector<int> num;
#endif

  bool food;
  void clear()
  {
    food = 0;
    num.clear();
  }
  void qdl()
  {
    while (!num.empty() && !num.back())
      num.pop_back();
  }

  bint(const int _num) { *this = _num; }
  bint(const long long _num) { *this = _num; }
  bint(const double _num) { *this = (long long)_num; }
  bint(const long double _num) { *this = (long long)_num; }
  bint(const char *_num)
  {
    food = false;
    _push(_num);
  }
  bint(const std::string _num)
  {
    food = false;
    _push(_num);
  }
  //	bint(const size_t _num) {*this = (long long)_num;}
  bint(const long _num) { *this = (int)_num; }
  bint(const unsigned long _num) { *this = (long long)_num; }
  bint(const unsigned long long _num) { *this = (long long)_num; }
  operator bool() const { return num.size(); }
  operator int() const
  {
    int f = food ? -1 : 1;
    int k = 0;
    for (auto u = (int)num.size() - 1; u >= 0; u--)
    {
      int v = num[u];
      k = ((k << 10) - (k << 4) - (k << 3)) + v;
    }
    // for(auto u:this->num) k = k * 1000 + u;
    return k * f;
  }
  operator long long() const
  {
    int f = food ? -1 : 1;
    long long k = 0;
    for (auto u = (int)num.size() - 1; u >= 0; u--)
    {
      int v = num[u];
      k = ((k << 10) - (k << 4) - (k << 3)) + v;
    }
    return k * f;
  }
  operator double() const
  {
    int f = food ? -1 : 1;
    double k = 0;
    for (auto u = (int)num.size() - 1; u >= 0; u--)
    {
      int v = num[u];
      k = k * 1000.0 + v;
    }
    return k * f;
  }
  operator long double() const
  {
    int f = food ? -1 : 1;
    long double k = 0;
    for (auto u = (int)num.size() - 1; u >= 0; u--)
    {
      int v = num[u];
      k = k * 1000.0 + v;
    }
    return k * f;
  }
  size_t len()
  {
    qdl();
    return num.size();
  }
  short operator[](const size_t &_nums) const
  {
    if (_nums > num.size() * 3)
      return false;
    return num[num.size() - _nums];
  }
  void _push(const std::string s)
  {
    _push(s.c_str());
    return;
  }
  void _push(const char *s)
  {
    int l = std::strlen(s), _begin = 0;
    while (s[_begin] == '-' && l > 1)
      _begin++, food = !food;
    while (s[_begin] == '0' && l > 1)
      _begin++;
    int i = l - 1;
    for (i = l - 1; i > _begin + 2; i -= 3)
    {
      short _num = 0;
      for (int j = i - 2; j <= i; j++)
      {
        _num = (_num << 1) + (_num << 3) + (s[j] ^ 48);
      }
      num.push_back(_num);
    }
    if (i >= 0)
    {
      short _num = 0;
      for (int j = _begin; j <= i; j++)
      {
        _num = (_num << 1) + (_num << 3) + (s[j] ^ 48);
      }
      if (_num)
        num.push_back(_num);
    }
  }
  bint jueduizhi()
  {
    bint t = *this;
    t.food = false;
    return t;
  }
  //	template <typename T>
  //	void operator = (T _num)
  //	{
  //		if(_num < 0) food = true , _num = - _num;
  //		//string s = to_string(_num);
  //		string s;
  //		while(_num)
  //		{
  //			s = (char(_num % 10 ^ 48)) + s;
  //			_num /= 10;
  //		}
  //		clear();
  //		_push(s);
  //		return ;
  //	}被废弃的int赋值

  friend std::istream &operator>>(std::istream &is, bint &_n)
  {
    _n.clear();
    std::string s;
    while (!std::cin.eof() && !isdigit(std::cin.peek()))
      if (std::cin.get() == '-')
        _n.food = !_n.food;
    is >> s;
    while (s.front() == '0' && s.size() > 1)
      s = s.substr(1);
    int l = s.size();
    int i = l - 1;
    for (i = l - 1; i > 2; i -= 3)
    {
      short _num = 0;
      for (int j = i - 2; j <= i; j++)
      {
        _num = (_num << 1) + (_num << 3) + (s[j] ^ 48);
      }
      _n.num.push_back(_num);
    }
    if (i >= 0)
    {
      short _num = 0;
      for (int j = 0; j <= i; j++)
      {
        _num = (_num << 1) + (_num << 3) + (s[j] ^ 48);
      }
      if (_num)
        _n.num.push_back(_num);
    }

    return is;
  }
  friend std::ostream &operator<<(std::ostream &os, const bint &_n)
  {
    const int l = _n.num.size();
    if (!l)
    {
      os << 0;
      return os;
    }
    if (_n.food)
      std::cout.put('-');
    os << _n.num[l - 1];
    for (int i = l - 2; i >= 0; i--)
      os << std::setw(3) << std::right << std::setfill('0') << _n.num[i];
    return os;
  }
  struct Compare_type
  {
    typedef short __cmp_return_type;
    __cmp_return_type operator()(const bint &A, const bint &B) const
    {
      if (A.food != B.food)
        return A.food ? -1 : 1;
      if (A.num.size() != B.num.size())
        return A.food ? (A.num.size() < B.num.size() ? 1 : -1) : (A.num.size() < B.num.size() ? -1 : 1);
      int siz = A.num.size();
      for (int i = siz - 1; i >= 0; i--)
      {
        if (A.num[i] != B.num[i])
          return A.food ? (A.num[i] < B.num[i] ? 1 : -1) : (A.num[i] < B.num[i] ? -1 : 1);
      }
      return 0;
    }
  } __cmp;
  bool operator<(const bint &Q) const
  {
    return __cmp(*this, Q) == -1;
  }
  bool operator<=(const bint &Q) const
  {
    return __cmp(*this, Q) <= 0;
  }
  bool operator==(const bint &Q) const
  {
    return __cmp(*this, Q) == 0;
  }
  bool operator>=(const bint &Q) const
  {
    return __cmp(*this, Q) >= 0;
  }
  bool operator>(const bint &Q) const
  {
    return __cmp(*this, Q) == 1;
  }
  bool operator!=(const bint &Q) const
  {
    return __cmp(*this, Q);
  }
  bint operator-() const
  {
    bint Q = *this;
    Q.food = !Q.food;
    return Q;
  }
  friend bool operator!(bint Q)
  {
    Q.qdl();
    return !Q.num.size();
  }
  bint operator++(int)
  {
    bint tmp = *this;
    ++(*this);
    return tmp;
  }
  bint operator++()
  {
    if (!(*this))
    {
      num.push_back(1);
    }
    else if (!food)
    {
      int l = num.size();
      num.front()++;
      for (int i = 0; i < l - 1; i++)
      {
        if (num[i] > 999)
        {
          num[i] -= 1000;
          num[i + 1]++;
        }
        else
          break;
      }
      if (num.back() > 999)
      {
        num.back() -= 1000;
        num.push_back(1);
      }
    }
    else
    {
      int l = num.size();
      num.front()--;
      for (int i = 0; i < l - 1; i++)
      {
        if (num[i] < 0)
        {
          num[i] += 1000;
          num[i + 1]--;
        }
        else
          break;
      }
    }
    qdl();
    return *this;
  }
  bint operator--()
  {
    if (!(*this))
    {
      food = 1;
      num.push_back(1);
    }
    else
    {
      food = !food;
      ++(*this);
      food = !food;
    }
    return *this;
  }
  bint operator--(int)
  {
    bint tmp = *this;
    --(*this);
    return tmp;
  }

  bint operator+(const bint &Q) const
  {
    bint ans;
    if (-(*this) == Q)
      return ans;
    int lQ = Q.num.size(), lthis = num.size();
    if (Q.food & food)
      ans.food = true;
    else if (Q.food && !food)
      return *this - (-Q);
    else if (!Q.food && food)
      return Q - -(*this);
    int lmax = std::max(lQ, lthis);
    ans.num.push_back(0);
    short tmp;
    for (int i = 1; i <= lmax; i++)
    {
      if (i <= lQ)
        ans.num.back() += Q.num[i - 1];
      if (i <= lthis)
        ans.num.back() += num[i - 1];
      if (ans.num.back() >= 1000)
      {
        tmp = ans.num.back();
        ans.num.back() %= 1000;
        ans.num.push_back(tmp / 1000);
      }
      else
        ans.num.push_back(0);
    }
    ans.qdl();
    return ans;
  }
  bint operator-(const bint &Q) const
  {
    bint ans;
    if (food ^ Q.food)
    {
      if (food)
        return -(-*this + Q);
      else
        return *this + (-Q);
    }
    else if (food)
      return -Q - (-*this);
    if (*this < Q)
    {
      ans = Q - *this;
      ans.food = true;
      return ans;
    }
    int lQ = Q.num.size(), lthis = num.size();
    ans.num.push_back(0);
    for (int i = 0; i < lthis; i++)
    {
      if (i < lQ)
        ans.num.back() += num[i] - Q.num[i];
      else
        ans.num.back() += num[i];
      if (ans.num.back() < 0)
      {
        int tmp = ans.num.back();
        ans.num.back() = tmp + 1000;
        ans.num.push_back(-1);
      }
      else
        ans.num.push_back(0);
    }
    ans.qdl();
    return ans;
  }
  bint operator<<(const bint &Q) const
  {
    if (!*this)
      return *this;
    bint ans;
    bint tmp = Q;
    ans = *this;
    const int _kong = 0;
    while (!ans.num.empty() && tmp--)
      ans.num.insert(ans.num.begin(), _kong);
    return ans;
  }
  bint operator<<=(const bint &Q)
  {
    if (!*this)
      return *this;
    bint tmp = Q;
    const int _kong = 0;
    while (tmp--)
      num.insert(num.begin(), _kong);
    return *this;
  }

  bint operator<<(int wei) const
  {
    if (!wei)
      return *this;
    bint k = *this;
    const int _kong = 0;
    for (int i = 1; i <= wei; i++)
      k.num.insert(k.num.begin(), _kong);
    return k;
  }
  void operator<<=(int wei)
  {
    if (!wei)
      return;
    int _kong = 0;
    for (int i = 1; i <= wei; i++)
      num.insert(num.begin(), _kong);
    return;
  }
  bint operator>>(const bint &wei) const
  {
    bint ans;
    if (!wei)
      return *this;
    if (wei >= (int)num.size())
      return ans;
    bint k = *this, tmp = wei;
    while (!k.num.empty() && tmp--)
      k.num.erase(k.num.begin());
    return k;
  }
  bint operator>>=(const bint &wei)
  {
    if (!wei)
      return *this;
    if (wei >= (int)num.size())
      return (bint)0;
    bint tmp = wei;
    while (!num.empty() && tmp--)
      num.erase(num.begin());
    return *this;
  }
  bint operator>>(int wei) const
  {
    if (!wei)
      return *this;
    bint k = *this;
    while (!k.num.empty() && wei--)
      k.num.erase(k.num.begin());
    return k;
  }
  void operator>>=(int wei)
  {
    if (!wei)
      return;
    while (!num.empty() && wei--)
      num.erase(num.begin());
    return;
  }
#ifdef FFT_MULTI

  void FFT_base_copy_to(vector<Comp> &_copy_to) const
  {
    for (size_t i = 0u; i != num.size(); i++)
      _copy_to[i] = (Comp(num[i], 0));
    // for (size_t i = num.size(); i != _copy_to.size(); i++)
    //   _copy_to[i] = (0);
    return;
  }
  void FFT_base_copy_by(const vector<Comp> &_copy_by, const size_t _len, const int _n)
  {
    for (size_t i = 0u; i != _len; i++)
      num.push_back(_copy_by[i].x / _n + 0.7);
    return tohead();
  }
  friend bint FFT_base(const bint &A, const bint &B)
  {
    const int la = (int)A.num.size(), lb = (int)B.num.size();

    const int lc = la + lb;
    bint ans;
    int _n;
    for (_n = 1; _n < lc; _n <<= 1)
      ;

    vector<Comp> _fa(_n + 1, 0), _fb(_n + 1, 0);

    A.FFT_base_copy_to(_fa);
    B.FFT_base_copy_to(_fb);

    FFT_base_virt_change(_n, _fa, false);
    FFT_base_virt_change(_n, _fb, false);

    for (int i = 0; i != _n; i++)
      _fa[i] = _fa[i] * _fb[i];

    FFT_base_virt_change(_n, _fa, true);
    return ans.FFT_base_copy_by(_fa, lc - 1, (size_t)_n), ans;
  }
#endif
  bint operator*(const bint &Q) const
  {
    int lQ = Q.num.size(), lthis = num.size();
    bint ans;
    if (Q.food ^ food)
      ans.food = true;
    if (!Q || !(*this))
      return ans;
#ifdef FFT_MULTI
    if (min(lQ, lthis) >= 5000)
    {
      return ans.food ? -FFT_base(*this, Q) : FFT_base(*this, Q);
    }
#endif
    ans.num.resize(lQ + lthis);
    if (std::min(lQ, lthis) > 1000) // 2 * 1024 * 1000 * 1000逼近INT_MAX
    {
      for (int i = 0; i < lQ; i++)
      {
        if (!(i & 1023))
          ans.tohead(); // x & 1023 == x % 1024
        for (int j = 0; j < lthis; j++)
          ans.num[i + j] += Q.num[i] * num[j];
      }
    }
    else
    {
      for (int i = 0; i < lQ; i++)
        for (int j = 0; j < lthis; j++)
          ans.num[i + j] += Q.num[i] * num[j];
    }
    ans.tohead();
    ans.qdl();
    return ans;
  }
  bint operator/(const bint Q) const
  {
    bool f;
    bint tmp, ans;
    if (!(*this))
      return ans;
    bint t1 = *this, t2 = Q;
    int lt2 = Q.num.size(), lt1 = num.size();
    int deltal = lt1 - lt2;
    f = food ^ Q.food;
    t1.food = t2.food = false;

    if (deltal < 0)
      return ans;
    const bint wei1 = 1;
    t2 <<= deltal;
    while (~deltal)
    {
      if (!t1 || !t2)
        break;
      int _l = 1, _r = 1000, _mid = 0;
      while (_l < _r)
      {
        _mid = (_l + _r) >> 1;
        if (t2 * _mid <= t1)
          _l = _mid + 1; // 草，一直没发现这里是小于等于
        else
          _r = _mid;
      }
      _l--;
      t1 -= _l * t2;
      ans += ((bint)_l << deltal);
      t2 >>= 1;
      deltal--;
    } // 除法的时候你压位越狠反倒越慢
    // 现在除法也非常快了,粗略的算一下,复杂度大概为O(log2(k1) * log(1000)(*this) * log2(*this)), 1 <= k1 < 1000
    return (f ? -ans : ans);
    //    	if(t1 < t2) return ans;
    //    	for(int i = lt1 - 1; i >= lt1-lt2; i--)
    //    		if(num[i] < Q.num[i - lt1 + lt2])
    //    			goto CONTINUE;
    //    	tmp = t2 << lt1 - lt2;
    //    	while(t1 >= tmp)
    //    	{
    //    		t1 = t1 - tmp;
    //    		k++;
    //		}
    //		ans = k;
    //		ans.qdl();
    //		ans = (ans << lt1 - lt2) + t1 / t2;
    //		return (f? -ans : ans);
    //		CONTINUE:;
    //		tmp = t2 << lt1 - lt2 - 1;
    //    	while(t1 >= tmp)
    //    	{
    //    		t1 = t1 - tmp;
    //    		k++;
    //		}
    //		ans = k;
    //		ans.qdl();
    //		ans = (ans << lt1 - lt2 - 1) + t1 / t2;
    //		return (f ? -ans : ans);被废弃的极慢的除法
  }
  bint operator%(const bint &Q) const
  {
    bint tmp = *this / Q;
    tmp = tmp * Q;
    return *this - tmp;
  }
  void operator+=(const bint &Q)
  {
    *this = *this + Q;
    return;
  }
  void operator-=(const bint &Q)
  {
    *this = *this - Q;
    return;
  }
  void operator*=(const bint &Q)
  {
    *this = *this * Q;
    return;
  }
  void operator/=(const bint &Q)
  {
    *this = *this / Q;
    return;
  }
  void operator%=(const bint &Q)
  {
    *this = *this % Q;
    return;
  }
  void operator=(int _num)
  {
    clear();
    if (_num < 0)
      _num = -_num, food = true;
    while (_num)
    {
      num.push_back(_num % 1000);
      _num /= 1000;
    }
    return;
  }

  void operator=(const char *_s)
  {
    return clear(), _push(_s), void();
  }

  void operator=(long long _num)
  {
    clear();
    if (_num < 0)
      _num = -_num, food = true;
    while (_num)
    {
      num.push_back(_num % 1000);
      _num /= 1000;
    }
    return;
  }
  template <typename T>
  void operator=(const T &_num)
  {
    clear();
    *this = _num;
    return;
  }
  template <typename T>
  bool operator<(const T &_num) const { return *this < (bint)_num; }
  template <typename T>
  bool operator<=(const T &_num) const { return *this <= (bint)_num; }
  template <typename T>
  bool operator>(const T &_num) const { return *this > (bint)_num; }
  template <typename T>
  bool operator>=(const T &_num) const { return *this >= (bint)_num; }
  template <typename T>
  bool operator==(const T &_num) const { return *this == (bint)_num; }
  template <typename T>
  bool operator!=(const T &_num) const { return *this != (bint)_num; }
  template <typename T>
  friend bool operator>=(const T &_num, const bint &Q) { return Q <= (bint)_num; }
  template <typename T>
  friend bool operator<=(const T &_num, const bint &Q) { return (bint)_num <= Q; }
  template <typename T>
  friend bool operator>(const T &_num, const bint &Q) { return Q < (bint)_num; }
  template <typename T>
  friend bool operator<(const T &_num, const bint &Q) { return Q > (bint)_num; }
  template <typename T>
  friend bool operator==(const T &_num, const bint &Q) { return Q == (bint)_num; }
  template <typename T>
  friend bool operator!=(const T &_num, const bint &Q) { return Q != (bint)_num; }
  template <typename T>
  bint operator+(const T &_num) const { return *this + (bint)_num; }
  template <typename T>
  bint operator+=(const T &_num) { return *this = *this + (bint)_num; }
  template <typename T>
  bint operator-(const T &_num) const { return *this - (bint)_num; }
  template <typename T>
  bint operator-=(const T &_num) { return *this = *this - (bint)_num; }
  template <typename T>
  bint operator*(const T &_num) const { return *this * (bint)_num; }
  template <typename T>
  bint operator*=(const T &_num) { return *this = *this * (bint)_num; }
  template <typename T>
  bint operator/(const T &_num) const { return *this / (bint)_num; }
  template <typename T>
  bint operator/=(const T &_num) { return *this = *this / (bint)_num; }
  template <typename T>
  bint operator%(const T &_num) const { return *this % (bint)_num; }
  template <typename T>
  bint operator%=(const T &_num) { return *this = *this % (bint)_num; }
  template <typename T>
  friend bint operator+(const T &_num, const bint &Q) { return (bint)_num + Q; }
  template <typename T>
  friend bint operator+=(T &_num, const bint &Q) { return _num = (bint)_num + Q; }
  template <typename T>
  friend bint operator-(const T &_num, const bint &Q) { return (bint)_num - Q; }
  template <typename T>
  friend bint operator-=(T &_num, const bint &Q) { return _num = (bint)_num - Q; }
  template <typename T>
  friend bint operator*(const T &_num, const bint &Q) { return (bint)_num * Q; }
  template <typename T>
  friend bint operator*=(T &_num, const bint &Q) { return _num = (bint)_num * Q; }
  template <typename T>
  friend bint operator/(const T &_num, const bint &Q) { return (bint)_num / Q; }
  template <typename T>
  friend bint operator/=(T &_num, const bint &Q) { return _num = (bint)_num / Q; }
  template <typename T>
  friend bint operator%(const T &_num, const bint &Q) { return (bint)_num % Q; }
  template <typename T>
  friend bint operator%=(T &_num, const bint &Q) { return _num = (bint)_num % Q; }
  /*--------------------extra part-------------------------*/
  void tohead() // 将进位写成函数，最好不内联.
  {
    int _siz = num.size();
    for (int i = 0; i != _siz; i++)
    {
      if (num[i] >= 1000)
      {
        if (i == _siz - 1)
        {
          num.push_back(num[i] / 1000);
          num[i] %= 1000;
          continue;
        }
        else
        {
          num[i + 1] += num[i] / 1000;
          num[i] %= 1000;
        }
      }
    }
    return;
  }
#if defined(NEED_POW) || defined(FFT_MULTI)
  bint power_base(const int base, const int index) const
  {
    bint ans = 1;
    long long con_mulnum = 1ll;
    int mul_num = log(INT_MAX) / log(base);
    for (int i = 1; i <= mul_num; i++)
      con_mulnum *= base;
    bint con_mulnum_bint = con_mulnum;
    bint base_bint = base;
    for (int i = index / mul_num; i >= 1; i--)
      ans *= con_mulnum_bint;
    for (int i = index % mul_num; i >= 1; i--)
      ans *= base_bint;
    return ans;
  }
  bint operator^(int index) const
  {
    if (*this < 0)
      return index & 1 ? -((-*this) ^ index) : (-*this ^ index);
    if (*this <= 1)
      return *this;
    if (*this <= 100 && index <= 100)
    {
      int tmp = *this;
      return power_base(tmp, index);
    }
    bint res = 1, ans = *this;
    while (index)
    {
      if (index & 1)
        res *= ans;
      index >>= 1;

      ans *= ans;
    }
    return res;
  }
#endif
};
// 新科技
bint operator"" _big(const unsigned long long _Q)
{
  return (bint)_Q;
}
bint operator"" _big(const char *_s, const std::size_t L) noexcept
{
#ifdef __ASSERT_H_
  assert(strlen(_s) == L);
#endif
  return (bint)_s;
}
#endif
