#ifndef FAST_SORT_HPP
#define FAST_SORT_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <cstring>
#include <iterator>
#include <cstdint>

// --- 核心内部逻辑：基数混合排序 ---
namespace internal {

    // 获取无符号映射类型，确保所有整数类型都有对应的无符号类型进行位操作
    template<typename T>
    struct UnsignedMap {
        typedef typename std::conditional<std::is_same<T, bool>::value, 
                        uint8_t, 
                        typename std::make_unsigned<T>::type>::type type;
    };

#ifdef __SIZEOF_INT128__
    template<> struct UnsignedMap<__int128> { typedef unsigned __int128 type; };
    template<> struct UnsignedMap<unsigned __int128> { typedef unsigned __int128 type; };
#endif

    // 执行位转换：处理负数补码，使其在无符号下保持单调递增
    template<typename T>
    inline typename UnsignedMap<T>::type encode(T val) {
        typedef typename UnsignedMap<T>::type U;
        U uval = static_cast<U>(val);
        if (std::is_signed<T>::value) {
            // 将符号位取反：负数从 0 开始，正数紧随其后
            return uval ^ (U(1) << (sizeof(T) * 8 - 1));
        }
        return uval;
    }

    template<typename T>
    inline T decode(typename UnsignedMap<T>::type uval) {
        typedef typename UnsignedMap<T>::type U;
        if (std::is_signed<T>::value) {
            uval ^= (U(1) << (sizeof(T) * 8 - 1));
        }
        return static_cast<T>(uval);
    }

    // 核心基数排序逻辑 (11-bit 桶，极致优化版)
    template<typename T>
    void radix_sort_impl(T* begin, T* end, bool descending) {
        const size_t n = static_cast<size_t>(end - begin);
        if (n <= 1) return;

        typedef typename UnsignedMap<T>::type U;
        
        std::unique_ptr<U[]> udata_ptr(new U[n]);
        std::unique_ptr<U[]> buffer_ptr(new U[n]);
        U* udata = udata_ptr.get();
        U* buffer = buffer_ptr.get();
        
        const int RADIX_BITS = 11;
        const int RADIX = 1 << RADIX_BITS;
        const int ITERATIONS = (sizeof(T) * 8 + RADIX_BITS - 1) / RADIX_BITS;
        
        // 统计表：32位需3层，64位需6层。统一分配以支持不同类型。
        uint32_t counts[ITERATIONS][RADIX];
        std::memset(counts, 0, sizeof(counts));

        // 第一步：单次遍历完成位转换和所有 11-bit 段的桶统计
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            U v0 = encode(begin[i]);
            U v1 = encode(begin[i+1]);
            U v2 = encode(begin[i+2]);
            U v3 = encode(begin[i+3]);
            udata[i] = v0; udata[i+1] = v1; udata[i+2] = v2; udata[i+3] = v3;
            for (int j = 0; j < ITERATIONS; ++j) {
                int shift = j * RADIX_BITS;
                counts[j][(v0 >> shift) & (RADIX - 1)]++;
                counts[j][(v1 >> shift) & (RADIX - 1)]++;
                counts[j][(v2 >> shift) & (RADIX - 1)]++;
                counts[j][(v3 >> shift) & (RADIX - 1)]++;
            }
        }
        for (; i < n; ++i) {
            U val = encode(begin[i]);
            udata[i] = val;
            for (int j = 0; j < ITERATIONS; ++j) {
                counts[j][(val >> (j * RADIX_BITS)) & (RADIX - 1)]++;
            }
        }

        // 第二步：计算前缀和
        for (int j = 0; j < ITERATIONS; ++j) {
            for (int k = 1; k < RADIX; ++k) {
                counts[j][k] += counts[j][k-1];
            }
        }

        U* src = udata;
        U* dst = buffer;

        // 第三步：分层搬运
        for (int j = 0; j < ITERATIONS; ++j) {
            uint32_t* cur_cnt = counts[j];
            int shift = j * RADIX_BITS;
            int mask = RADIX - 1;
            U* p = src + n;
            size_t m = n >> 3;
            while (m--) {
                dst[--cur_cnt[(p[-1] >> shift) & mask]] = p[-1];
                dst[--cur_cnt[(p[-2] >> shift) & mask]] = p[-2];
                dst[--cur_cnt[(p[-3] >> shift) & mask]] = p[-3];
                dst[--cur_cnt[(p[-4] >> shift) & mask]] = p[-4];
                dst[--cur_cnt[(p[-5] >> shift) & mask]] = p[-5];
                dst[--cur_cnt[(p[-6] >> shift) & mask]] = p[-6];
                dst[--cur_cnt[(p[-7] >> shift) & mask]] = p[-7];
                dst[--cur_cnt[(p[-8] >> shift) & mask]] = p[-8];
                p -= 8;
            }
            switch (n & 7) {
                case 7: dst[--cur_cnt[(p[-1] >> shift) & mask]] = p[-1]; p--;
                case 6: dst[--cur_cnt[(p[-1] >> shift) & mask]] = p[-1]; p--;
                case 5: dst[--cur_cnt[(p[-1] >> shift) & mask]] = p[-1]; p--;
                case 4: dst[--cur_cnt[(p[-1] >> shift) & mask]] = p[-1]; p--;
                case 3: dst[--cur_cnt[(p[-1] >> shift) & mask]] = p[-1]; p--;
                case 2: dst[--cur_cnt[(p[-1] >> shift) & mask]] = p[-1]; p--;
                case 1: dst[--cur_cnt[(p[-1] >> shift) & mask]] = p[-1]; p--;
            }
            std::swap(src, dst);
        }

        // 写回结果
        if (descending) {
            for (size_t i = 0; i < n; ++i) begin[i] = decode<T>(src[n - 1 - i]);
        } else {
            for (size_t i = 0; i < n; ++i) begin[i] = decode<T>(src[i]);
        }
    }

    // 通过重载分发，处理指针和迭代器逻辑（模拟 if constexpr）
    template<typename T, typename RandomIt>
    inline void fast_sort_dispatch(RandomIt begin, RandomIt end, bool descending, std::true_type) {
        // 指针情况
        internal::radix_sort_impl(begin, end, descending);
    }

    template<typename T, typename RandomIt>
    inline void fast_sort_dispatch(RandomIt begin, RandomIt end, bool descending, std::false_type) {
        // 非指针迭代器情况
        size_t n = static_cast<size_t>(std::distance(begin, end));
        std::vector<T> temp(begin, end);
        internal::radix_sort_impl(&temp[0], &temp[0] + n, descending);
        std::copy(temp.begin(), temp.end(), begin);
    }
}

// --- fast_sort 接口 ---

// 判断是否为标准比较器
template<typename T, typename Comp> struct is_standard_compare : std::false_type {};
template<typename T> struct is_standard_compare<T, std::less<T> > : std::true_type {};
template<typename T> struct is_standard_compare<T, std::greater<T> > : std::true_type {};

#if __cplusplus >= 201402L
template<typename T> struct is_standard_compare<T, std::less<void> > : std::true_type {};
template<typename T> struct is_standard_compare<T, std::greater<void> > : std::true_type {};
#endif

/**
 * fast_sort: 兼容负数、长整数的高速排序
 * 逻辑：针对整数使用 16-bit 基数映射算法，其余情况回退到 std::sort
 */
template<typename RandomIt, typename Compare>
void fast_sort(RandomIt begin, RandomIt end, Compare comp) {
    typedef typename std::iterator_traits<RandomIt>::value_type T;
    auto n_dist = std::distance(begin, end);
    if (n_dist <= 1) return;
    size_t n = static_cast<size_t>(n_dist);

    if (n < 128 || !std::is_integral<T>::value || !is_standard_compare<T, Compare>::value) {
        std::sort(begin, end, comp);
        return;
    }

    bool descending = std::is_same<Compare, std::greater<T> >::value;
#if __cplusplus >= 201402L
    descending = descending || std::is_same<Compare, std::greater<void> >::value;
#endif
    
    // 使用 std::is_pointer 进行分发
    internal::fast_sort_dispatch<T>(begin, end, descending, typename std::is_pointer<RandomIt>::type());
}

// 默认参数重载
template<typename RandomIt>
void fast_sort(RandomIt begin, RandomIt end) {
    typedef typename std::iterator_traits<RandomIt>::value_type T;
    fast_sort(begin, end, std::less<T>());
}

#ifndef NO_FAST_SORT_OVERLOAD
#undef sort
#define sort fast_sort
#endif

#endif
