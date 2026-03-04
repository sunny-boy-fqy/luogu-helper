/*
Fast I/O library - Cross platform (Linux/Windows/macOS)
Uses mmap on Linux, file mapping on Windows, and fallback to buffered I/O
*/

#ifndef QIO_HPP
#define QIO_HPP

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <cmath>
#include <sys/stat.h>
#include "bint.hpp"
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#ifdef __linux__
#include <sys/mman.h>
#endif
#endif

// ==================================================================================
// 辅助组件
// ==================================================================================

// 输出 2 位查表
struct OutputTable {
    char data[100][2];
    constexpr OutputTable() : data() {
        for (int i = 0; i < 100; ++i) {
            data[i][0] = (i / 10) + '0';
            data[i][1] = (i % 10) + '0';
        }
    }
};
static constexpr OutputTable output_table;

// 输入 2 位查表
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
static InputPre input_pre;

// ==================================================================================
// QInStream: 极速输入 (mmap + 查表)
// 注意：仅在文件重定向时启用优化，终端模式下使用标准 cin
// ==================================================================================
class QInStream {
private:
    static constexpr size_t LARGE_BUFFER_SIZE = 1 << 26; // 64MB
    char *m_p, *m_c, *m_end;
    bool m_is_mmap;
    FILE *m_file;
    bool m_use_fast_io;
    
    void refill() {
        if (m_file && !m_is_mmap && m_use_fast_io) {
            size_t n = fread(m_p, 1, LARGE_BUFFER_SIZE, m_file);
            m_p[n] = '\0';
            m_c = m_p;
            m_end = m_p + n;
        }
    }

public:
    QInStream(FILE *file = stdin) : m_p(nullptr), m_c(nullptr), m_end(nullptr), m_is_mmap(false), m_file(nullptr), m_use_fast_io(false) {
        bool is_tty = false;
#ifdef _WIN32
        int fd = _fileno(file);
        is_tty = _isatty(fd) != 0;
#else
        int fd = fileno(file);
        is_tty = isatty(fd) != 0;
#endif
        
        if (!is_tty) {
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
                            m_is_mmap = true;
                            m_c = m_p;
                            m_end = m_p + st.st_size;
                            m_use_fast_io = true;
                            return;
                        }
                    }
                }
#elif defined(__linux__)
                m_p = (char *)mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                if (m_p != MAP_FAILED) {
                    m_is_mmap = true;
                    m_c = m_p;
                    m_end = m_p + st.st_size;
                    m_use_fast_io = true;
                    return;
                }
#endif
            }
            
            m_file = file;
            m_p = new char[LARGE_BUFFER_SIZE + 1];
            size_t n = fread(m_p, 1, LARGE_BUFFER_SIZE, file);
            m_p[n] = '\0';
            m_c = m_p;
            m_end = m_p + n;
            m_use_fast_io = true;
        }
    }

    ~QInStream() {
        if (m_p) {
#ifdef _WIN32
            if (m_is_mmap) UnmapViewOfFile(m_p);
            else
#elif defined(__linux__)
            if (m_is_mmap) munmap(m_p, static_cast<size_t>(m_end - m_p));
            else
#endif
            delete[] m_p;
        }
    }

    template <typename Tp, typename std::enable_if<std::is_integral<Tp>::value>::type * = nullptr>
    QInStream &operator>>(Tp &x) {
        if (!m_use_fast_io) {
            std::cin >> x;
            return *this;
        }
        x = 0;
        if (m_c >= m_end) {
            refill();
        }
        char *c = m_c;
        char *end = m_end;
        while (c < end && ((unsigned char)*c < '0' || (unsigned char)*c > '9') && *c != '-') ++c;
        if (c >= end) { m_c = c; return *this; }
        bool neg = false;
        if (std::is_signed<Tp>::value && *c == '-') { neg = true; ++c; }
        while (c < end && (unsigned char)*c >= '0' && (unsigned char)*c <= '9') {
            if (c + 1 < end) {
                uint16_t val = (uint16_t)((unsigned char)c[0] | ((unsigned char)c[1] << 8));
                uint32_t res = input_pre.m_data[val];
                if (res != 0xFFFFFFFFu) {
                    x = x * 100 + res;
                    c += 2;
                    continue;
                }
            }
            x = x * 10 + ((unsigned char)*c++ - '0');
        }
        if (neg) x = -x;
        m_c = c;
        return *this;
    }

    QInStream &operator>>(char &x) {
        if (!m_use_fast_io) {
            std::cin >> x;
            return *this;
        }
        if (m_c >= m_end) {
            refill();
        }
        while (m_c < m_end && (unsigned char)*m_c <= ' ') m_c++;
        if (m_c < m_end) x = *m_c++;
        return *this;
    }

    QInStream &operator>>(std::string &x) {
        if (!m_use_fast_io) {
            std::cin >> x;
            return *this;
        }
        x.clear();
        if (m_c >= m_end) {
            refill();
        }
        char *c = m_c;
        char *end = m_end;
        while (c < end && (unsigned char)*c <= ' ') ++c;
        if (c >= end) { m_c = c; return *this; }
        char *start = c;
        while (c < end && (unsigned char)*c > ' ') ++c;
        x.assign(start, c - start);
        m_c = c;
        return *this;
    }

    QInStream &operator>>(double &x) {
        if (!m_use_fast_io) {
            std::cin >> x;
            return *this;
        }
        if (m_c >= m_end) {
            refill();
        }
        double t = 0.0;
        int sign = 1;
        char *c = m_c;
        char *end = m_end;
        while (c < end && !((unsigned char)*c >= '0' && (unsigned char)*c <= '9') && *c != '-' && *c != '+' && *c != '.') ++c;
        if (c >= end) { x = 0.0; m_c = c; return *this; }
        if (*c == '-') { sign = -1; ++c; }
        else if (*c == '+') { ++c; }

        bool any = false;
        while (c < end && (unsigned char)*c >= '0' && (unsigned char)*c <= '9') { any = true; t = t * 10.0 + ((unsigned char)*c++ - '0'); }
        if (c < end && *c == '.') {
            ++c;
            double frac = 0.1;
            while (c < end && (unsigned char)*c >= '0' && (unsigned char)*c <= '9') { any = true; t += ((unsigned char)*c++ - '0') * frac; frac *= 0.1; }
        }

        if (!any) { x = 0.0; m_c = c; return *this; }

        if (c < end && (*c == 'e' || *c == 'E')) {
            ++c;
            int exp_sign = 1;
            if (c < end && (*c == '+' || *c == '-')) { if (*c == '-') exp_sign = -1; ++c; }
            int exp = 0;
            while (c < end && (unsigned char)*c >= '0' && (unsigned char)*c <= '9') { exp = exp * 10 + ((unsigned char)*c++ - '0'); }
            if (exp != 0) {
                int e = exp_sign * exp;
                double p = 1.0;
                double base = 10.0;
                int ae = e < 0 ? -e : e;
                while (ae) {
                    if (ae & 1) p *= base;
                    base *= base;
                    ae >>= 1;
                }
                if (e < 0) t /= p; else t *= p;
            }
        }

        x = t * sign;
        m_c = c;
        return *this;
    }
    QInStream &operator>>(bint &x) {
        if (!m_use_fast_io) {
            std::cin >> x;
            return *this;
        }
        std::string value;
        *this >> value;
        if (!value.empty()) x = value;
        return *this;
    }
    QInStream &operator>>(ubint &x) {
        if (!m_use_fast_io) {
            std::cin >> x;
            return *this;
        }
        std::string value;
        *this >> value;
        if (!value.empty()) x = value;
        return *this;
    }
    void tie(void*) {}
};

// ==================================================================================
// QOutStream: 极速输出 (大缓冲 + 2位查表)
// ==================================================================================
class QOutStream {
private:
    static constexpr size_t OUTPUT_BUFFER_SIZE = 1 << 20; // 1MB
    FILE *m_file;
    char m_buf[OUTPUT_BUFFER_SIZE], *m_c, *m_end;

public:
    QOutStream(FILE *file = stdout) : m_file(file) {
        m_c = m_buf;
        m_end = m_buf + OUTPUT_BUFFER_SIZE;
    }

    ~QOutStream() { flush(); }

    void flush() {
        if (m_c != m_buf) {
            fwrite(m_buf, 1, m_c - m_buf, m_file);
            m_c = m_buf;
        }
    }

    inline void put(char x) {
        if (m_c == m_end) flush();
        *m_c++ = x;
    }

    QOutStream &operator<<(char x) { put(x); return *this; }

    QOutStream &operator<<(const char *s) {
        if (!s) return *this;
        size_t len = strlen(s);
        size_t avail = static_cast<size_t>(m_end - m_c);
        if (len <= avail) {
            memcpy(m_c, s, len);
            m_c += len;
        } else {
            const char *p = s;
            while (*p) { if (m_c == m_end) flush(); *m_c++ = *p++; }
        }
        return *this;
    }
    
    QOutStream &operator<<(const std::string &s) {
        size_t len = s.size();
        size_t avail = static_cast<size_t>(m_end - m_c);
        if (len <= avail) {
            memcpy(m_c, s.data(), len);
            m_c += len;
        } else {
            for (char c : s) put(c);
        }
        return *this;
    }

    template <typename Tp, typename std::enable_if<std::is_integral<Tp>::value>::type * = nullptr>
    QOutStream &operator<<(Tp x) {
        if (m_end - m_c < 24) flush();
        if (x == 0) { put('0'); return *this; }
        uint64_t val;
        if (std::is_signed<Tp>::value && x < 0) {
            put('-');
            int64_t sx = static_cast<int64_t>(x);
            uint64_t u = static_cast<uint64_t>(-(sx + 1)) + 1; // safe for INT64_MIN
            val = u;
        } else {
            val = static_cast<uint64_t>(x);
        }
        
        char tmp[24];
        int len = 0;
        while (val >= 100) {
            int idx = val % 100;
            val /= 100;
            tmp[len++] = output_table.data[idx][1];
            tmp[len++] = output_table.data[idx][0];
        }
        if (val >= 10) {
            tmp[len++] = output_table.data[val][1];
            tmp[len++] = output_table.data[val][0];
        } else {
            tmp[len++] = (char)(val + '0');
        }
        while (len--) put(tmp[len]);
        return *this;
    }

    QOutStream &operator<<(double x) {
        char tmp[40];
        int len = sprintf(tmp, "%.10f", x); 
        if (m_end - m_c < len) flush();
        memcpy(m_c, tmp, len);
        m_c += len;
        return *this;
    }
    
    QOutStream &operator<<(bint x) {
        const char* value = static_cast<const char*>(x);
        return *this << value;
    }
    
    void tie(void*) {}
};

// ==================================================================================
// 全局实例与宏
// ==================================================================================
static QInStream qin;
static QOutStream qout;
static const char qendl = '\n';

#ifndef QIO_NO_OVERRIDE
#define cin qin
#define cout qout
#define endl qendl
#endif

#define ios _patched_ios
namespace _patched_ios {
    inline void sync_with_stdio(bool) {}
}

#endif // QIO_HPP