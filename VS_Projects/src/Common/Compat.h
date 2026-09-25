#ifndef __ROSE_COMPAT_H__
#define __ROSE_COMPAT_H__

#ifndef _WIN32
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <cstdarg>
#include <string>
#include <chrono>

// Windows clock() measures in milliseconds with CLOCKS_PER_SEC = 1000.
// Emulate Windows millisecond clock on Linux to keep game timers, intervals, and fuel rates accurate.
#undef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000
inline clock_t rose_clock() {
    using namespace std::chrono;
    static const auto start_time = steady_clock::now();
    return (clock_t)duration_cast<milliseconds>(steady_clock::now() - start_time).count();
}
#define clock() rose_clock()

// Fixed-width integer types for 64-bit Linux compatibility
typedef unsigned int        UINT;
typedef int                 BOOL;
typedef uint8_t             BYTE;
typedef int16_t             SWORD;
typedef uint16_t            WORD;
typedef int32_t             SDWORD;
typedef uint32_t            DWORD;
typedef int64_t             SQWORD;
typedef uint64_t            QWORD;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// QSD Types
typedef uint8_t             byte;
typedef uint16_t            word;
typedef uint32_t            dword;
typedef uint64_t            qword;
typedef char*               strings;

// Case-insensitive string comparisons
#ifndef _stricmp
#define _stricmp strcasecmp
#endif
#ifndef _strnicmp
#define _strnicmp strncasecmp
#endif
#ifndef _strcmpi
#define _strcmpi strcasecmp
#endif
#ifndef _strdup
#define _strdup strdup
#endif

inline char* rose_strdate(char* buf) {
    if (!buf) return NULL;
    time_t t = time(NULL);
    struct tm ti;
    localtime_r(&t, &ti);
    strftime(buf, 9, "%m/%d/%y", &ti);
    return buf;
}
#ifndef _strdate
#define _strdate rose_strdate
#endif

// Sleep in milliseconds
inline void rose_sleep_ms(unsigned int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}
#ifndef Sleep
#define Sleep(ms) rose_sleep_ms(ms)
#endif

// MSVC CRT _s functions
#ifndef sprintf_s
#define sprintf_s(buf, ...) sprintf(buf, __VA_ARGS__)
#endif
#ifndef vsprintf_s
#define vsprintf_s(buf, fmt, args) vsprintf(buf, fmt, args)
#endif
#ifndef fscanf_s
#define fscanf_s(stream, format, ...) fscanf(stream, format, __VA_ARGS__)
#endif
#ifndef sscanf_s
#define sscanf_s(str, format, ...) sscanf(str, format, __VA_ARGS__)
#endif
#ifndef strcpy_s
#define strcpy_s(dst, src) strcpy(dst, src)
#endif
#ifndef strncpy_s
#define strncpy_s(dst, src, count) strncpy(dst, src, count)
#endif

inline int ctime_s(char* buf, size_t bufsz, const time_t* timer) {
    if (!buf || bufsz < 26) return -1;
    return ctime_r(timer, buf) ? 0 : -1;
}

// Windows SYSTEMTIME emulation
struct SYSTEMTIME {
    uint16_t wYear;
    uint16_t wMonth;
    uint16_t wDayOfWeek;
    uint16_t wDay;
    uint16_t wHour;
    uint16_t wMinute;
    uint16_t wSecond;
    uint16_t wMilliseconds;
};

inline void GetLocalTime(SYSTEMTIME* st) {
    if (!st) return;
    time_t rawtime;
    time(&rawtime);
    struct tm ti;
    localtime_r(&rawtime, &ti);
    st->wYear = (uint16_t)(ti.tm_year + 1900);
    st->wMonth = (uint16_t)(ti.tm_mon + 1);
    st->wDayOfWeek = (uint16_t)ti.tm_wday;
    st->wDay = (uint16_t)ti.tm_mday;
    st->wHour = (uint16_t)ti.tm_hour;
    st->wMinute = (uint16_t)ti.tm_min;
    st->wSecond = (uint16_t)ti.tm_sec;
    st->wMilliseconds = 0;
}

// Transparent path normalization and case-insensitive file open
FILE* rose_fopen(const char* filename, const char* mode);

#ifndef fopen
#define fopen rose_fopen
#endif

inline int fopen_s(FILE** pFile, const char* filename, const char* mode) {
    if (!pFile) return -1;
    *pFile = rose_fopen(filename, mode);
    return *pFile ? 0 : -1;
}

#endif // !_WIN32

#endif // __ROSE_COMPAT_H__
