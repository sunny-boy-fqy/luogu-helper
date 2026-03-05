/*
 * Fast I/O library - Cross platform (Linux/Windows/macOS)
 * Supports C++98, C++11, and above.
 * Automatically detects compiler version for BigInt (bint) support.
 */

#ifndef QIO_HPP
#define QIO_HPP

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>
#include <sys/stat.h>
#include <iostream>

// ==================================================================================
// 编译器特性检测与大数类集成
// ==================================================================================

// 只有 G++ >= 4.8 且开启 C++11 时才尝试包含 bint.hpp
#undef QIO_HAS_BINT
#if defined(__GNUC__)
    #if (__GNUC__ * 100 + __GNUC_MINOR__ >= 408) && (__cplusplus >= 201103L)
        #define QIO_HAS_BINT 1
    #endif
#elif defined(_MSC_VER)
    #if _MSC_VER >= 1900 // VS 2015+
        #define QIO_HAS_BINT 1
    #endif
#endif

// 如果用户手动定义了 QIO_NO_BINT，则强制关闭
#ifdef QIO_NO_BINT
    #undef QIO_HAS_BINT
#endif

#ifdef QIO_HAS_BINT
    #include "bint.hpp"
#endif

// ==================================================================================
// 兼容性类型定义 (针对 C++98)
// ==================================================================================
#if defined(_MSC_VER) && _MSC_VER < 1600
    typedef unsigned __int32 uint32_t;
    typedef unsigned __int16 uint16_t;
    typedef unsigned __int64 uint64_t;
    typedef __int64          int64_t;
#else
    #include <stdint.h>
#endif

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #define Q_ISATTY _isatty
    #define Q_FILENO _fileno
#else
    #include <unistd.h>
    #define Q_ISATTY isatty
    #define Q_FILENO fileno
    #ifdef __linux__
        #include <sys/mman.h>
    #endif
#endif

// ==================================================================================
// 辅助组件：类型识别与查表
// ==================================================================================

// 简单的类型识别 (替代 C++11 type_traits)
template<typename T> struct QIsIntegral { static const bool value = false; };
template<> struct QIsIntegral<int> { static const bool value = true; };
template<> struct QIsIntegral<long> { static const bool value = true; };
template<> struct QIsIntegral<short> { static const bool value = true; };
template<> struct QIsIntegral<unsigned int> { static const bool value = true; };
template<> struct QIsIntegral<unsigned long> { static const bool value = true; };
template<> struct QIsIntegral<signed char> { static const bool value = true; };
template<> struct QIsIntegral<unsigned char> { static const bool value = true; };
#if defined(__GNUC__) || defined(_MSC_VER) // 支持 long long
template<> struct QIsIntegral<long long> { static const bool value = true; };
template<> struct QIsIntegral<unsigned long long> { static const bool value = true; };
#endif

template<typename T> struct QIsSigned { static const bool value = false; };
template<> struct QIsSigned<int> { static const bool value = true; };
template<> struct QIsSigned<long> { static const bool value = true; };
template<> struct QIsSigned<short> { static const bool value = true; };
template<> struct QIsSigned<signed char> { static const bool value = true; };
#if defined(__GNUC__) || defined(_MSC_VER)
template<> struct QIsSigned<long long> { static const bool value = true; };
#endif

struct OutputTable {
    char data[100][2];
    OutputTable() {
        for (int i = 0; i < 100; ++i) {
            data[i][0] = (char)((i / 10) + '0');
            data[i][1] = (char)((i % 10) + '0');
        }
    }
};
static const OutputTable output_table;

struct InputPre {
    uint32_t m_data[0x10000];
    InputPre() {
        memset(m_data, 0xFF, sizeof(m_data));
        for (uint32_t i = 0; i < 10; ++i) {
            for (uint32_t j = 0; j < 10; ++j) {
                m_data[0x3030 + i + (j << 8)] = i * 10 + j;
            }
        }
    }
};
static const InputPre input_pre;

// ==================================================================================
// QInStream
// ==================================================================================
class QInStream {
private:
    static const size_t BUFFER_SIZE = 1 << 26; // 64MB
    char *m_p, *m_c, *m_end;
    bool m_is_mmap;
    FILE *m_file;
    bool m_use_fast_io;
    
    void refill() {
        if (m_file && !m_is_mmap && m_use_fast_io) {
            size_t n = fread(m_p, 1, BUFFER_SIZE, m_file);
            m_p[n] = '\0';
            m_c = m_p;
            m_end = m_p + n;
        }
    }

public:
    QInStream(FILE *file = stdin) : m_p(NULL), m_c(NULL), m_end(NULL), m_is_mmap(false), m_file(NULL), m_use_fast_io(false) {
        int fd = Q_FILENO(file);
        if (!Q_ISATTY(fd)) {
            struct stat st;
            if (fstat(fd, &st) == 0 && st.st_size > 0) {
#ifdef _WIN32
                HANDLE hFile = (HANDLE)_get_osfhandle(fd);
                if (hFile != INVALID_HANDLE_VALUE) {
                    HANDLE hMap = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
                    if (hMap != NULL) {
                        m_p = (char *)MapViewOfFileEx(hMap, FILE_MAP_READ, 0, 0, 0, NULL);
                        CloseHandle(hMap);
                        if (m_p != NULL) {
                            m_is_mmap = true; m_c = m_p; m_end = m_p + st.st_size;
                            m_use_fast_io = true; return;
                        }
                    }
                }
#elif defined(__linux__)
                m_p = (char *)mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                if (m_p != MAP_FAILED) {
                    m_is_mmap = true; m_c = m_p; m_end = m_p + st.st_size;
                    m_use_fast_io = true; return;
                }
#endif
            }
            m_file = file;
            m_p = new char[BUFFER_SIZE + 1];
            size_t n = fread(m_p, 1, BUFFER_SIZE, file);
            m_p[n] = '\0'; m_c = m_p; m_end = m_p + n;
            m_use_fast_io = true;
        }
    }

    ~QInStream() {
        if (m_p) {
#ifdef _WIN32
            if (m_is_mmap) UnmapViewOfFile(m_p); else delete[] m_p;
#elif defined(__linux__)
            if (m_is_mmap) munmap(m_p, (size_t)(m_end - m_p)); else delete[] m_p;
#else
            delete[] m_p;
#endif
        }
    }

    template <typename Tp>
    QInStream &operator>>(Tp &x) {
        if (!m_use_fast_io) { std::cin >> x; return *this; }
        x = 0; if (m_c >= m_end) refill();
        char *c = m_c, *end = m_end;
        while (c < end && (*c < '0' || *c > '9') && *c != '-') ++c;
        if (c >= end) { m_c = c; return *this; }
        bool neg = false;
        if (QIsSigned<Tp>::value && *c == '-') { neg = true; ++c; }
        while (c < end && *c >= '0' && *c <= '9') {
            if (c + 1 < end) {
                uint16_t val = (uint16_t)((unsigned char)c[0] | ((unsigned char)c[1] << 8));
                uint32_t res = input_pre.m_data[val];
                if (res != 0xFFFFFFFFu) { x = x * 100 + res; c += 2; continue; }
            }
            x = x * 10 + (*c++ - '0');
        }
        if (neg) x = -x; m_c = c; return *this;
    }

    QInStream &operator>>(char &x) {
        if (!m_use_fast_io) { std::cin >> x; return *this; }
        if (m_c >= m_end) refill();
        while (m_c < m_end && (unsigned char)*m_c <= ' ') m_c++;
        if (m_c < m_end) x = *m_c++;
        return *this;
    }

    QInStream &operator>>(std::string &x) {
        if (!m_use_fast_io) { std::cin >> x; return *this; }
        x.clear(); if (m_c >= m_end) refill();
        char *c = m_c, *end = m_end;
        while (c < end && (unsigned char)*c <= ' ') ++c;
        if (c >= end) { m_c = c; return *this; }
        char *start = c;
        while (c < end && (unsigned char)*c > ' ') ++c;
        x.assign(start, (size_t)(c - start));
        m_c = c; return *this;
    }

#ifdef QIO_HAS_BINT
    QInStream &operator>>(bint &x) {
        std::string s; *this >> s; if(!s.empty()) x = s; return *this;
    }
    QInStream &operator>>(ubint &x) {
        std::string s; *this >> s; if(!s.empty()) x = s; return *this;
    }
#endif

    void tie(void*) {}
};

// ==================================================================================
// QOutStream
// ==================================================================================
class QOutStream {
private:
    static const size_t BUFFER_SIZE = 1 << 20; // 1MB
    FILE *m_file;
    char m_buf[BUFFER_SIZE], *m_c, *m_end;

public:
    QOutStream(FILE *file = stdout) : m_file(file) {
        m_c = m_buf; m_end = m_buf + BUFFER_SIZE;
    }
    ~QOutStream() { flush(); }

    void flush() {
        if (m_c != m_buf) {
            fwrite(m_buf, 1, (size_t)(m_c - m_buf), m_file);
            m_c = m_buf;
        }
    }

    inline void put(char x) { if (m_c == m_end) flush(); *m_c++ = x; }

    QOutStream &operator<<(char x) { put(x); return *this; }
    QOutStream &operator<<(const char *s) {
        if (!s) return *this;
        size_t len = strlen(s);
        if (len <= (size_t)(m_end - m_c)) { memcpy(m_c, s, len); m_c += len; }
        else { while (*s) put(*s++); }
        return *this;
    }
    
    QOutStream &operator<<(const std::string &s) {
        size_t len = s.size();
        if (len <= (size_t)(m_end - m_c)) { memcpy(m_c, s.data(), len); m_c += len; }
        else { for (size_t i = 0; i < len; ++i) put(s[i]); }
        return *this;
    }

    template <typename Tp>
    QOutStream &operator<<(Tp x) {
        if (!QIsIntegral<Tp>::value) return *this;
        if (m_end - m_c < 24) flush();
        if (x == 0) { put('0'); return *this; }
        uint64_t val;
        if (QIsSigned<Tp>::value && x < 0) {
            put('-');
            val = (uint64_t)-(x + 1) + 1;
        } else val = (uint64_t)x;
        
        char tmp[24]; int len = 0;
        while (val >= 100) {
            int idx = (int)(val % 100); val /= 100;
            tmp[len++] = output_table.data[idx][1];
            tmp[len++] = output_table.data[idx][0];
        }
        if (val >= 10) {
            tmp[len++] = output_table.data[(int)val][1];
            tmp[len++] = output_table.data[(int)val][0];
        } else tmp[len++] = (char)(val + '0');
        while (len--) put(tmp[len]);
        return *this;
    }

#ifdef QIO_HAS_BINT
    QOutStream &operator<<(const bint &x) {
        return *this << static_cast<const char*>(x);
    }
    QOutStream &operator<<(const ubint &x) {
        return *this << static_cast<const char*>(x);
    }
#endif

    void tie(void*) {}
};

// ==================================================================================
// 全局实例
// ==================================================================================
static QInStream qin(stdin);
static QOutStream qout(stdout);
static const char qendl = '\n';

#ifndef QIO_NO_OVERRIDE
    #define cin qin
    #define cout qout
    #define endl qendl
#endif

#endif // QIO_HPP