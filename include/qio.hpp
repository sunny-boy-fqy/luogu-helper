/*
 * Fast I/O library - Cross platform
 * Supports C++98, C++11, and above.
 * Condition-based BigInt support without hardcoded includes.
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
// 1. 自动环境检测 (不强制 include)
// ==================================================================================
#undef QIO_CAN_SUPPORT_BINT
#if defined(__GNUC__)
    #if (__GNUC__ * 100 + __GNUC_MINOR__ >= 408) && (__cplusplus >= 201103L)
        #define QIO_CAN_SUPPORT_BINT 1
    #endif
#elif defined(_MSC_VER)
    #if _MSC_VER >= 1900 
        #define QIO_CAN_SUPPORT_BINT 1
    #endif
#endif

// ==================================================================================
// 2. 基础类型与平台适配
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
// 3. 类型识别
// ==================================================================================
template<typename T> struct QIsIntegral { static const bool value = false; };
template<typename T> struct QIsSigned { static const bool value = false; };

#define Q_REG_INT(Type, Signed) \
    template<> struct QIsIntegral<Type> { static const bool value = true; }; \
    template<> struct QIsSigned<Type> { static const bool value = Signed; };

Q_REG_INT(int, true) Q_REG_INT(long, true) Q_REG_INT(short, true) Q_REG_INT(signed char, true)
Q_REG_INT(unsigned int, false) Q_REG_INT(unsigned long, false) Q_REG_INT(unsigned short, false) Q_REG_INT(unsigned char, false)
#if defined(__GNUC__) || defined(_MSC_VER)
    Q_REG_INT(long long, true) Q_REG_INT(unsigned long long, false)
#endif
#ifdef __SIZEOF_INT128__
    Q_REG_INT(__int128, true) Q_REG_INT(unsigned __int128, false)
#endif

// ==================================================================================
// 4. 辅助查表
// ==================================================================================
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
        for (uint32_t i = 0; i < 10; ++i)
            for (uint32_t j = 0; j < 10; ++j)
                m_data[0x3030 + i + (j << 8)] = i * 10 + j;
    }
};
static const InputPre input_pre;

// ==================================================================================
// QInStream
// ==================================================================================
class QInStream {
private:
    static const size_t BUFFER_SIZE = 1 << 26; 
    char *m_p, *m_c, *m_end;
    bool m_is_mmap;
    FILE *m_file;
    bool m_use_fast_io;
    
    void refill() {
        if (m_file && !m_is_mmap && m_use_fast_io) {
            size_t n = fread(m_p, 1, BUFFER_SIZE, m_file);
            m_p[n] = '\0'; m_c = m_p; m_end = m_p + n;
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
                        if (m_p) { m_is_mmap = true; m_c = m_p; m_end = m_p + st.st_size; m_use_fast_io = true; return; }
                    }
                }
#elif defined(__linux__)
                m_p = (char *)mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                if (m_p != MAP_FAILED) { m_is_mmap = true; m_c = m_p; m_end = m_p + st.st_size; m_use_fast_io = true; return; }
#endif
            }
            m_file = file; m_p = new char[BUFFER_SIZE + 1];
            size_t n = fread(m_p, 1, BUFFER_SIZE, file);
            m_p[n] = '\0'; m_c = m_p; m_end = m_p + n; m_use_fast_io = true;
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

    inline int get_char() {
        if (!m_use_fast_io) return std::cin.get();
        if (m_c >= m_end) { refill(); if (m_c >= m_end) return EOF; }
        return (unsigned char)*m_c++;
    }

    template <typename Tp>
    QInStream& operator>>(Tp &x) {
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
        int c = get_char(); while (c != EOF && (unsigned char)c <= ' ') c = get_char();
        if (c != EOF) x = (char)c; return *this;
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

    // 只有在外部已经定义了 bint 类型时才激活此重载
#ifdef QIO_HAS_BINT
    QInStream &operator>>(bint &x) {
        std::string s; *this >> s; if (!s.empty()) x = s; return *this;
    }
    QInStream &operator>>(ubint &x) {
        std::string s; *this >> s; if (!s.empty()) x = s; return *this;
    }
#endif

    void tie(void* p = NULL) {}
};

// ==================================================================================
// QOutStream
// ==================================================================================
class QOutStream {
private:
    static const size_t BUFFER_SIZE = 1 << 20;
    FILE *m_file;
    char m_buf[BUFFER_SIZE], *m_c, *m_end;
    bool m_use_fast_io;

public:
    QOutStream(FILE *file = stdout) : m_file(file) {
        m_c = m_buf; m_end = m_buf + BUFFER_SIZE;
        int fd = Q_FILENO(file);
        m_use_fast_io = !Q_ISATTY(fd);
    }
    ~QOutStream() { flush(); }

    void flush() {
        if (!m_use_fast_io) { std::cout.flush(); return; }
        if (m_c != m_buf) { fwrite(m_buf, 1, (size_t)(m_c - m_buf), m_file); m_c = m_buf; }
    }

    inline void put_char(char x) { 
        if (!m_use_fast_io) { std::cout.put(x); return; }
        if (m_c == m_end) flush(); *m_c++ = x; 
    }

    template <typename Tp>
    QOutStream &operator<<(Tp x) {
        if (!m_use_fast_io) { std::cout << x; return *this; }
        if (m_end - m_c < 64) flush(); 
        if (x == 0) { put_char('0'); return *this; }
        
        if (QIsSigned<Tp>::value && x < 0) {
            put_char('-');
#ifdef __SIZEOF_INT128__
            unsigned __int128 val = (unsigned __int128)-(x + 1) + 1;
#else
            uint64_t val = (uint64_t)-(x + 1) + 1;
#endif
            write_unsigned(val);
        } else {
            write_unsigned(x);
        }
        return *this;
    }

    template <typename UTp>
    void write_unsigned(UTp val) {
        char tmp[128]; int len = 0;
        while (val >= 100) {
            int idx = (int)(val % 100); val /= 100;
            tmp[len++] = output_table.data[idx][1];
            tmp[len++] = output_table.data[idx][0];
        }
        if (val >= 10) {
            tmp[len++] = output_table.data[(int)val][1];
            tmp[len++] = output_table.data[(int)val][0];
        } else tmp[len++] = (char)(val + '0');
        while (len--) put_char(tmp[len]);
    }

    QOutStream &operator<<(char x) { put_char(x); return *this; }
    QOutStream &operator<<(const char *s) { 
        if (!m_use_fast_io) { if(s) std::cout << s; return *this; }
        if (s) while (*s) put_char(*s++); 
        return *this; 
    }
    QOutStream &operator<<(const std::string &s) { 
        if (!m_use_fast_io) { std::cout << s; return *this; }
        for (size_t i = 0; i < s.size(); ++i) put_char(s[i]); 
        return *this; 
    }

    // 只有在外部已经定义了 bint 类型时才激活此重载
#ifdef QIO_HAS_BINT
    QOutStream &operator<<(const bint &x) {
        // 利用 x 提供的 operator std::string()
        return *this << static_cast<std::string>(x);
    }
    QOutStream &operator<<(const ubint &x) {
        return *this << static_cast<std::string>(x);
    }
#endif

    void tie(void* p = NULL) {}
};

static QInStream qin(stdin);
static QOutStream qout(stdout);

#undef getchar
#undef putchar
#define getchar() qin.get_char()
#define putchar(c) qout.put_char(c)

#ifndef QIO_NO_OVERRIDE
    #define cin qin
    #define cout qout
    #define endl '\n'
#endif

#endif // QIO_HPP
