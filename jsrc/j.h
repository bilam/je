/* Copyright 1990-2014, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Global Definitions                                                      */

#undef C_AVX2
#undef C_AVX512
#undef EMU_AVX2
#undef PYXES
#undef SLEEF
#undef SLEEFQUAD
#undef MEMAUDIT
#define C_AVX2 0
#define C_AVX512 0
#define EMU_AVX2 0
#define PYXES 0
#define SLEEF 0
#define SLEEFQUAD 0
#define MEMAUDIT 0

#if defined(__clang_major__) && !defined(__clang__)
#error need workaround by define __clang__ in preprocessor macro
#endif

/* clang-cl */
#if defined(__clang__) && !defined(__GNUC__)
#define __GNUC__ 4
#undef __GNUC_MINOR__
#define __GNUC_MINOR__ 2
#undef __GNUC_PATCHLEVEL__
#define __GNUC_PATCHLEVEL__ 1
#endif

// ms vc++ defined _MSC_VER but clang-cl also defined _MSC_VER
// clang-cl doesn't emulate ms vc++ good enough
// and it breaks program logic previously guarded by _MSC_VER
// MMSC_VER means the real ms vc++ excluding clang-cl
// use MMSC_VER instead of _MSC_VER throughout JE source
#if defined(_MSC_VER) && !defined(__clang__)
#undef MMSC_VER
#define MMSC_VER
#else
#undef MMSC_VER
#endif
#if !defined(MMSC_VER)
#include <stddef.h>       // offsetof
#endif

/* msvc does not define __SSE2__ */
#if !defined(__SSE2__)
#if defined(MMSC_VER)
#if (defined(_M_AMD64) || defined(_M_X64))
#define __SSE2__ 1
#elif _M_IX86_FP==2
#define __SSE2__ 1
#endif
#endif
#endif
#if defined(__SSE2__)
#include <emmintrin.h>
#endif

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

// for debugging
#define NANTEST0        (fetestexcept(FE_INVALID))  // test but does not clear
#define dump_m128i(a,x) {__m128i _b=x;fprintf(stderr,"%s %x %x %x %x \n", a, ((unsigned int*)(&_b))[0], ((unsigned int*)(&_b))[1], ((unsigned int*)(&_b))[2], ((unsigned int*)(&_b))[3]);}
#define dump_m128i64(a,x) {__m128i _b=x;fprintf(stderr,"%s %lli %lli \n", a, ((long long*)(&_b))[0], ((long long*)(&_b))[1]);}
#define dump_m256i(a,x) {__m256i _b=x;fprintf(stderr,"%s %lli %lli %lli %lli \n", a, ((long long*)(&_b))[0], ((long long*)(&_b))[1], ((long long*)(&_b))[2], ((long long*)(&_b))[3]);}
#define dump_m256i16(a,x) {__m256i _b=x;fprintf(stderr,"%s %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \n", a, ((unsigned short*)(&_b))[0], ((unsigned short*)(&_b))[1], ((unsigned short*)(&_b))[2], ((unsigned short*)(&_b))[3], ((unsigned short*)(&_b))[4], ((unsigned short*)(&_b))[5], ((unsigned short*)(&_b))[6], ((unsigned short*)(&_b))[7],((unsigned short*)(&_b))[8], ((unsigned short*)(&_b))[9], ((unsigned short*)(&_b))[10], ((unsigned short*)(&_b))[11], ((unsigned short*)(&_b))[12], ((unsigned short*)(&_b))[13], ((unsigned short*)(&_b))[14], ((unsigned short*)(&_b))[15]);}
#define dump_m256i32(a,x) {__m256i _b=x;fprintf(stderr,"%s %x %x %x %x %x %x %x %x \n", a, ((unsigned int*)(&_b))[0], ((unsigned int*)(&_b))[1], ((unsigned int*)(&_b))[2], ((unsigned int*)(&_b))[3], ((unsigned int*)(&_b))[4], ((unsigned int*)(&_b))[5], ((unsigned int*)(&_b))[6], ((unsigned int*)(&_b))[7]);}
#define dump_m256d(a,x) {__m256d _b=x;fprintf(stderr,"%s %f %f %f %f \n", a, ((double*)(&_b))[0], ((double*)(&_b))[1], ((double*)(&_b))[2], ((double*)(&_b))[3]);}
#define dump_m128d(a,x) {__m128d _b=x;fprintf(stderr,"%s %f %f \n", a, ((double*)(&_b))[0], ((double*)(&_b))[1]);}


#ifdef MMSC_VER
#define NOINLINE __declspec(noinline)
#define INLINE __forceinline
#else
#define NOINLINE __attribute__((noinline))
#define INLINE inline __attribute__((__always_inline__))
#endif
#ifdef __MINGW32__
// original definition
// #define INLINE extern __inline__ __attribute__((__always_inline__,__gnu_inline__))
#undef INLINE
#define INLINE __inline__ __attribute__((__always_inline__,__gnu_inline__))
#endif

#if defined(_WIN64)||defined(__LP64__)
#ifndef C_AVX512
#define C_AVX512 0
#endif
#ifndef C_AVX2
#define C_AVX2 0
#endif
#if C_AVX512
#undef C_AVX2
#define C_AVX2 1
#endif
#else
#undef C_AVX512
#define C_AVX512 0
#undef C_AVX2
#define C_AVX2 0
#undef EMU_AVX2
#define EMU_AVX2 0
#endif

#ifdef _WIN32
#if C_AVX2 || EMU_AVX2
#ifndef _WIN64
#error not 64-bit compiler
#endif
#endif
#endif

#if C_AVX2
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))
#include <immintrin.h>
#endif
#endif

#if !(defined(__x86_64__) || defined(__aarch64__) || defined(_M_ARM64))
#undef EMU_AVX2
#define EMU_AVX2 0
#endif

#if !defined(EMU_AVX2) && ((defined(__SSE2__) && defined(__x86_64__)) || defined(__aarch64__) || defined(_M_ARM64))
#undef EMU_AVX2
#define EMU_AVX2 1
#endif

#ifndef EMU_AVX2
#define EMU_AVX2 0
#endif

// no EMU_AVX512; avx512 is not widespread yet, and older chips still downclock (so not worth it for small arrays), so still maintain avx2-specific paths

#if C_AVX2
#undef EMU_AVX2
#define EMU_AVX2 0
#elif defined(__SSE2__) && defined(__x86_64__)
#if EMU_AVX2
#include <stdint.h>
#include <string.h>
#include "avxintrin-emu.h"
//#include "avx2intrin-emu.h"
#else
#include <emmintrin.h>
#endif
#define _CMP_EQ          0
#define _CMP_LT          1
#define _CMP_LE          2
#define _CMP_UNORD       3
#define _CMP_NEQ         4
#define _CMP_NLT         5
#define _CMP_NLE         6
#define _CMP_ORD         7
#undef _CMP_EQ_OQ
#undef _CMP_GE_OQ
#undef _CMP_GT_OQ
#undef _CMP_LE_OQ
#undef _CMP_LT_OQ
#undef _CMP_NEQ_OQ
#define _CMP_EQ_OQ _CMP_EQ
#define _CMP_GE_OQ _CMP_NLT
#define _CMP_GT_OQ _CMP_NLE
#define _CMP_LE_OQ _CMP_LE
#define _CMP_LT_OQ _CMP_LT
#define _CMP_NEQ_OQ _CMP_NEQ
#endif //__SSE2__

#if defined(__aarch64__)||defined(_M_ARM64)
#if EMU_AVX2
#undef SSE2NEON_SUPPRESS_WARNINGS
#define SSE2NEON_SUPPRESS_WARNINGS
#include <stdint.h>
#include <string.h>
#include "sse2neon.h"
#include "sse2neon2.h"
#include "avxintrin-neon.h"
#else
#include <arm_neon.h>
#endif
#endif

#if SLEEF
#undef SLEEFQUAD
#define SLEEFQUAD 1
#endif

#if SLEEF && !defined(_CMP_EQ)
#define _CMP_EQ          0
#define _CMP_LT          1
#define _CMP_LE          2
#define _CMP_UNORD       3
#define _CMP_NEQ         4
#define _CMP_NLT         5
#define _CMP_NLE         6
#define _CMP_ORD         7
#undef _CMP_EQ_OQ
#undef _CMP_GE_OQ
#undef _CMP_GT_OQ
#undef _CMP_LE_OQ
#undef _CMP_LT_OQ
#undef _CMP_NEQ_OQ
#define _CMP_EQ_OQ _CMP_EQ
#define _CMP_GE_OQ _CMP_NLT
#define _CMP_GT_OQ _CMP_NLE
#define _CMP_LE_OQ _CMP_LE
#define _CMP_LT_OQ _CMP_LT
#define _CMP_NEQ_OQ _CMP_NEQ
#endif

#if defined(__arm__)
#if defined(__ARM_NEON)
#include <arm_neon.h>
typedef double float64x2_t __attribute__ ((vector_size (16)));
#else
#include <stdint.h>
typedef int64_t int64x2_t __attribute__ ((vector_size (16)));
typedef double float64x2_t __attribute__ ((vector_size (16)));
#endif
#endif

#undef VOIDARG
#define VOIDARG

#if C_AVX512
#if (!defined(__clang__)) && defined(__GNUC__) && __GNUC__ < 10
static __inline __m512i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_loadu_epi64 (void const *__P)
{
  struct __loadu_epi64 {
    __m512i_u __v;
  } __attribute__((__packed__, __may_alias__));
  return ((const struct __loadu_epi64*)__P)->__v;
}

static __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_storeu_epi64 (void *__P, __m512i __A)
{
  struct __storeu_epi64 {
    __m512i_u __v;
  } __attribute__((__packed__, __may_alias__));
  ((struct __storeu_epi64*)__P)->__v = __A;
}
#endif
#endif

#if defined(__AVX2__) || defined(__aarch64__)   // note can't do #if x defined(y)
#define HASFMA 1  // true if architecture has hardware FMA capacity with AVX2 instructions
#else
#define HASFMA 0
#endif

#if defined(_OPENMP)
#include <omp.h>
#else
typedef int omp_int_t;
static inline omp_int_t omp_get_thread_num() { return 0;}
static inline omp_int_t omp_get_max_threads() { return 1;}
static inline omp_int_t omp_get_num_threads() { return 1;}
#endif

#ifndef NO_UNDEFINED
#define NO_UNDEFINED 0
#endif

#ifndef SYS // include js.h only once - dtoa.c
#include "js.h"
#endif

// todo look into whether windows supports this or not; I have heard support is spotty?
#if C_AVX512 && (SY_FREEBSD || SY_LINUX)
#define C_FSGSBASE 1
#else
#define C_FSGSBASE 0
#endif

// If you are porting to a new compiler or architecture, see the bottom of this file
// for instructions on defining the CTTZ macros

#if SY_WINCE
#include "..\cesrc\cecompat.h"
#endif

#if (SYS & SYS_PCWIN)
#define HEAPCHECK  heapcheck()
#else
#define HEAPCHECK
#endif

#if (SYS & SYS_ATARIST)
#define __NO_INLINE__           1
#endif

#if (SYS & SYS_UNIX - SYS_SGI)
#include <unistd.h>
#include <memory.h>
#include <sys/types.h>
#endif

// likely/unlikely support
#if defined(__clang__) || defined(__GNUC__)
#ifndef likely
#define likely(x) __builtin_expect(!!(x),1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x),0)
#endif
#if defined(_WIN32) || defined(__clang__) || __GNUC__ > 9
#if (defined(__has_builtin) && __has_builtin(__builtin_expect_with_probability)) || (!defined(__clang__) && __GNUC__ >= 9)
#define common(x) __builtin_expect_with_probability(!!(x),1,0.6)
#define uncommon(x) __builtin_expect_with_probability(!!(x),1,0.4)
#define withprob(x,p) __builtin_expect_with_probability(!!(x),1,(p))
#else
#define common(x) likely(x)
#define uncommon(x) unlikely(x)
#define withprob(x,p) (x)
#endif
#else
#define common(x) likely(x)
#define uncommon(x) unlikely(x)
#define withprob(x,p) (x)
#endif
#else
#define likely(x) (!!(x))
#define unlikely(x) (!!(x))
#define common(x) (!!(x))
#define uncommon(x) (!!(x))
#define withprob(x,p) (x)
#endif

#include <stdint.h>
#include <float.h>
#include <limits.h>
#define link unused_syscall_link
#define qdiv unused_netbsd_qdiv
#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K  // for posix_memalign
#endif
#include <stdlib.h>
#undef link
#undef qdiv

#if ! SY_WINCE
#include <errno.h>
#include <stdio.h>
#endif

#include <math.h>
#include <string.h>  

#ifdef ANDROID
#include <android/log.h>
#define logcat_d(msg) __android_log_write(ANDROID_LOG_DEBUG,(const char*)"libj",msg)
#endif

#if defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE||TARGET_OS_IOS||TARGET_OS_TV||TARGET_OS_WATCH||TARGET_OS_SIMULATOR||TARGET_OS_EMBEDDED||TARGET_IPHONE_SIMULATOR
#define TARGET_IOS 1
#endif
#endif

#if defined(__aarch32__)||defined(__arm__)||defined(_M_ARM)
// 32-bit arm only
#define ALIGNREQ 0b0110 // 2- and 4-byte loads must be aligned on a boundary of their size
#else
#define ALIGNREQ 0  // no alignment required
#endif

#if SY_WIN32
#if defined(_WIN32) && !defined(OLECOM)
#define OLECOM
#endif
#endif

#if SY_64
#define IMAX            9223372036854775807LL
#define IMAXPRIME       9223372036854775783LL
#define IMIN            (~9223372036854775807LL)   /* ANSI C LONG_MIN is  -LONG_MAX */
#define FLIMIN          ((D)IMIN)  // smallest FL value that can be converted to I
#define FMTI            "%lli"
#define FMTUI           "%llu"
#define FMTI02          "%02lli"
#define FMTI04          "%04lli"
#define FMTI05          "%05lli"
#define FMTX            "%llx"
#define UIMAX           -1ull

#if defined(MMSC_VER)  // SY_WIN32
#define strtoI         _strtoi64
#else
#define strtoI          strtoll
#endif

#else
#define IMAX            2147483647L
#define IMAXPRIME       IMAX
#define IMIN            (~2147483647L)   /* ANSI C LONG_MIN is  -LONG_MAX */
#define FLIMIN          ((D)IMIN)  // smallest FL value that can be converted to I
#define FMTI            "%d"
#define FMTUI           "%u"
#define FMTI02          "%02d"
#define FMTI04          "%04d"
#define FMTI05          "%05d"
#define FMTX            "%x"
#define UIMAX           -1ul
#define strtoI          strtol
#endif
#define FLIMAX          (-(D)IMIN)     // smallest FL value that cannot be converted to I

#define NEGATIVE0       (UIL)0x8000000000000000LL   // IEEE -0 (double precision)

#define C4MAX           0xffffffffUL
#define C4MIN           0L

#if (SYS & SYS_AMIGA)
#define XINF            "\177\377\000\000\000\000\000\000"
#define XNAN            "\177\361\000\000\000\000\000\000"
#endif

#if (SYS & SYS_ARCHIMEDES)
#define XINF            "\000\000\360\177\000\000\000\000"
#define XNAN            "\000\000\370\377\000\000\000\000"
#endif

#if (SYS & SYS_DEC5500) || SY_WINCE_SH
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\000\000\000\000\000\000\370\377"
#endif

#if (SYS & SYS_MACINTOSH)
/* for old versions of ThinkC */
/* #define XINF         "\177\377\000\000\000\000\000\000\000\000\000\000" */
/* #define XNAN         "\377\377\100\000\100\000\000\000\000\000\000\000" */
/* for ThinkC 7.0 or later */
#define XINF            "\177\377\177\377\000\000\000\000\000\000\000\000"
#define XNAN            "\377\377\377\377\100\000\000\000\000\000\000\000"
#endif

#if (SYS & SYS_SUN4+SYS_SUNSOL2)
#define XINF            "\177\360\000\000\000\000\000\000"
#define XNAN            "\177\377\377\377\377\377\377\377"
#endif

#if (SYS & SYS_VAX)
#define XINF            "\377\177\377\377\377\377\377\377"
#define XNAN            "\377\177\377\377\377\377\377\376" /* not right */
#endif

#if (SY_WINCE_MIPS || SY_WINCE_SH)
#if WIN32_PLATFORM_PSPC
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\377\377\377\377\377\377\367\177"
#else
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\001\000\000\000\000\000\360\177"
#endif
#endif

#if SY_WINCE_ARM
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\000\000\000\000\000\000\370\177"
#endif

#if SY_WASM
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\000\000\000\000\000\000\370\377"
#endif

#if C_LE
#ifndef XINF
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\000\000\000\000\000\000\370\377"
#endif
#endif

#ifndef XINF
#define XINF            "\177\360\000\000\000\000\000\000"
#define XNAN            "\177\370\000\000\000\000\000\000"
#endif


#ifndef PI
#define PI              ((D)3.14159265358979323846)
#endif
#define P2              ((D)6.28318530717958647693)
#ifndef OVERFLOW
#define OVERFLOW        ((D)8.988465674311578e307)
#endif
#ifndef UNDERFLOW
#define UNDERFLOW       ((D)4.450147717014403e-308)
#endif

// RESTRICT causes the compiler to generate better code by assuming no overlap of regions pointed to by pointers
// We use RESTRICT for routines that operate in-place on an argument.  This is strictly speaking a violation of the rule,
// but normally something like *z = *x + *y will not cause trouble because there is no reason to refetch an input after
// the result has been written.  On 32-bit machines, registers are so short that sometimes the compilers refetch an input
// after writing to *z, so we don't turn RESTRICT on for 32-bit
#if defined(MMSC_VER)
// RESTRICT is an attribute of a pointer, and indicates that no other pointer points to the same area
#define RESTRICT __restrict
// RESTRICTF is an attribute of a function, and indicates that the object returned by the function is not aliased with any other object
#define RESTRICTF __declspec(restrict)
#define PREFETCH(x) _mm_prefetch((x),_MM_HINT_T0)
#define PREFETCH2(x) _mm_prefetch((x),_MM_HINT_T1)   // prefetch into L2 cache but not L1
#elif defined(__GNUC__)
#define RESTRICT __restrict
#define RESTRICTF __attribute__((malloc))
#define PREFETCH(x) __builtin_prefetch(x)
#define PREFETCH2(x) __builtin_prefetch((x),0,2)   // prefetch into L2 cache but not L1
#else
#define RESTRICT
#define RESTRICTF
#define PREFETCH(x)
#define PREFETCH2(x)
#endif

#ifdef __MINGW32__
#ifndef _SW_INVALID
#define _SW_INVALID    0x00000010 /* invalid */
#endif
#ifndef _EM_ZERODIVIDE
#define _EM_ZERODIVIDE  0x00000008
#endif
#define EM_INVALID    _SW_INVALID
#define EM_ZERODIVIDE _EM_ZERODIVIDE
#if defined(__STRICT_ANSI__)
extern int __cdecl _isnan (double);
extern unsigned int __cdecl _clearfp (void);
#endif
#ifndef _MAX_PATH
#ifdef PATH_MAX
#define _MAX_PATH  PATH_MAX
#else
#define _MAX_PATH  (260)
#endif
#endif
#endif

#if SY_WIN32
struct jtimespec { long long tv_sec, tv_nsec; };
struct jtimeval { long long tv_sec, tv_usec; };
struct jtimezone { int tz_minuteswest, tz_dsttime; };
int jgettimeofday(struct jtimeval*, struct jtimezone*);
#else
#include <sys/time.h>
#include <time.h>
#define jtimespec timespec
#define jtimeval timeval
#define jtimezone timezone
#define jgettimeofday gettimeofday
#endif
struct jtimespec jmtclk(void); //monotonic clock.  Intended rel->abs conversions when sleeping; has poor granularity and slow on windows
struct jtimespec jmtfclk(void); //'fast clock'; maybe less inaccurate; intended for timed busywaiting

#if SY_64
#if defined(MMSC_VER)  // SY_WIN32
// RESTRICTI (for in-place) is used for things like *z++=*x++ - *y++;  Normally you wouldn't store to a z unless you were done reading
// the x and y, so it would be safe to get the faster loop that RESTRICT generates, even though strictly speaking if x or y is the
// same address as z the terms of the RESTRICT are violated.  But on 32-bit machines, registers are so tight that sometimes *z is used
// as a temp, which means we can't take the liberties there
#define RESTRICTI // __restrict don't take chances
#endif
#ifdef __GNUC__
#define RESTRICTI // __restrict  don't take chances
#endif
#endif  // SY_64

#ifndef RESTRICT
#define RESTRICT
#endif
#ifndef RESTRICTF
#define RESTRICTF
#endif
#ifndef RESTRICTI
#define RESTRICTI
#endif
// If PREFETCH is not defined, we won't generate prefetch instrs

// If the user switch C_NOMULTINTRINSIC is defined, suppress using it
#ifdef C_NOMULTINTRINSIC
#define C_USEMULTINTRINSIC 0
#else
#define C_USEMULTINTRINSIC 1
#endif

// disable C_USEMULTINTRINSIC if un-available
#if C_USEMULTINTRINSIC
#if !defined(MMSC_VER)
#if defined(__clang__)
#if !__has_builtin(__builtin_smul_overflow)
#undef C_USEMULTINTRINSIC
#define C_USEMULTINTRINSIC 0
#endif
#elif __GNUC__ < 5
#undef C_USEMULTINTRINSIC
#define C_USEMULTINTRINSIC 0
#endif
#endif
#endif

#if !SY_64 && defined(__GNUC__) && !defined(__clang__)
#if __GNUC__ < 5
#define __builtin_add_overflow(a,b,c) ({int64_t s=(int64_t)(a)+(int64_t)(b); *(c)=(long)s; (s<INT_MIN||s>INT_MAX);})
#define __builtin_sub_overflow(a,b,c) ({int64_t s=(int64_t)(a)-(int64_t)(b); *(c)=(long)s; (s<INT_MIN||s>INT_MAX);})
#define __builtin_mul_overflow(a,b,c) ({int64_t s=(int64_t)(a)*(int64_t)(b); *(c)=(long)s; (s<INT_MIN||s>INT_MAX);})
#endif
#endif

#if defined(__clang__) && ( (__clang_major__ > 3) || ((__clang_major__ == 3) && (__clang_minor__ > 5)))
/* needed by clang newer versions, no matter double_trick is inline asm or not */
#define NOOPTIMIZE __attribute__((optnone))
#elif __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 3))
#define NOOPTIMIZE __attribute__((optimize("O0")))
#else
#define NOOPTIMIZE
#endif

#define NALP            256             /* size of alphabet                */
#define NETX            2000            /* size of error display buffer    */
#define NPP             20              /* max value for quad pp           */
#define NPATH           1024            /* max length for path names,      */
                                        /* including trailing 0 byte       */
// Now we are trying to watch the C stack directly

// The named-call stack is used only when there is a locative, EXCEPT that after a call to 18!:4 it is used until the function calling 18!:4 returns.
// Since startup calls 18!:4 without a name, we have to allow for the possibility of deep recursion in the name stack.  Normally only a little of the stack is used
#if defined(CSTACKSIZE)  // some builds explicitly override the stacksize
#if !defined(CSTACKRESERVE)
#error CSTACKSIZE and CSTACKRESERVE must be defined together
#endif
#else
#if defined(_WIN32)
#define CSTACKSIZE      (SY_64?12009472:1015808)  // size we allocate in the calling function, aligned to 16k system page size  9961472 for 10MB
#else
#if (defined(ANDROID) && !defined(__LP64__)) || (defined(__OpenBSD__) && defined(__aarch64__))
#define CSTACKSIZE      (SY_64?4194304:1015808)  // OS default stack size 4MB, aligned to 16k system page size
#else
#define CSTACKSIZE      (SY_64?7946240:1015808)  // OS default stack size 8MB, aligned to 16k system page size
#endif
#endif
#define CSTACKRESERVE   (SY_64?200000:10000)  // amount we allow for slop before we sample the stackpointer, and after the last check
#endif
#define CSTACKDEBUGRESERVE (CSTACKRESERVE>>1)   // minimum to allow for debugger
//The named-function stack is intelligent
// and stacks only when there is a locale change or deletion; it almost never limits unless locatives are used to an extreme degree.
// The depth of the C stack will normally limit stack use.
// #define NFCALL          (1000L)      // call depth for named calls, not important.  Must fit into an S
#if SY_WINCE
#define NFCALL          100L            /* wince     named fn call depth   */
#define NFDEP           200L            /* wince           fn call depth   */
#endif
#if SYS & SYS_MACOSX
#define NFCALL           9000L          /* darwin    named fn call depth   */
#define NFDEP           18000L          /* darwin          fn call depth   */
#endif
#ifndef NFCALL
#define NFCALL          10000L          /* all other named fn call depth   */
#define NFDEP           20000L          /* all other       fn call depth   */
#endif

// start and length for the stored vector of ascending integers
#define IOTAVECBEGIN (-20)
#define IOTAVECLEN 400   // must be >= 256 so all memsets can be sourced from here

// #define NTSTACK         2000L           /* size of stack for temps         */
// tpop stack is allocated in units of NTSTACK, but processed in units of NTSTACKBLOCK on an NTSTACKBLOCK boundary to reduce waste in each allocation.
// If we audit execution results, we use a huge allocation so that tpop pointers can be guaranteed never to need a second one, & will thus be ordered
#define NTSTACK         BIT(14)          // number of BYTES in an allocated block of tstack - pointers to allocated blocks - allocation is bigger to leave this many bytes on boundary
#define NTSTACKBLOCK    2048            // boundary for beginning of stack block

// modes for indexofsub()
#define IIOPMSKX        5  // # bits of flags
#define IIDOT           0               /* modes for indexofsub()          */
#define IICO            1
#define INUBSV          2
#define INUB            3
#define ILESS           4
#define INUBI           5
#define IEPS            6
#define II0EPS          7
#define II1EPS          8
#define IJ0EPS          9
#define IJ1EPS          10
#define ISUMEPS         11
#define IANYEPS         12
#define IALLEPS         13
#define IIFBEPS         14

#define IPHOFFSET       30              /* offset for prehashed versions   */
#define IPHIDOT         30
#define IPHICO          31
#define IPHLESS         34
#define IPHEPS          36
#define IPHI0EPS        37
#define IPHI1EPS        38
#define IPHJ0EPS        39
#define IPHJ1EPS        40
#define IPHSUMEPS       41
#define IPHANYEPS       42
#define IPHALLEPS       43
#define IPHIFBEPS       44

#define jceil(x) ceil(x)
#define jfloor(x) floor(x)
#define jround(x) floor(0.5+(x))  // for paranoid compatibility with earlier versions

#define BB              8      /* # bits in a byte */
#define LGBB 3    // lg(BB)
#if SY_64
#define BW              64     /* # bits in a word */
#define LGSZI 3    // lg(#bytes in an I)
#define LGSZE 4   // lg(#bytes in an E)
#else
#define BW              32
#define LGSZI 2
#endif
#define LGBW (LGSZI+LGBB)  // lg (# bits in a word)

#ifndef PYXES
#if SY_64
#define PYXES 1
#else
#define PYXES 0
#endif
#endif

// if we are not multithreading, report the master thread only
#if !PYXES
#undef MAXTHREADS
#define MAXTHREADS 1  // override to no tasks if no pyxes
#endif
#if defined(ANDROID) && defined(__x86_64__)
#undef MAXTHREADS
#define MAXTHREADS 1  // workaround for android x86_64
#endif

#if PYXES
#define YIELD sched_yield();  // if we are spinning on other threads, give them a chance to run in case they might be on this core
#define REPATGCLIM 0x100000   // When this many bytes have been repatriated to a thread, call a GC in that thread
#define REPATOLIM (REPATGCLIM/32) // When an outgoing repatriation queue contains this many bytes, flush it
#else
// if we are not multithreading, we replace the atomic operations with non-atomic versions
#define YIELD ;   // if no other processes, no reason to delay
#define __atomic_store_n(aptr,val, memorder) (*aptr=val)
#define __atomic_load_n(aptr, memorder) *aptr
#if defined(__clang__) || __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 8))
#define __atomic_compare_exchange_n(aptr, aexpected, desired, weak, success_memorder, failure_memorder) (*aptr=desired,1)
#define __atomic_exchange_n(aptr, val, memorder) ({__auto_type rrres=*aptr; *aptr =val; rrres;})
#define __atomic_fetch_or(aptr, val, memorder)   ({__auto_type rrres=*aptr; *aptr|=val; rrres;})
#define __atomic_fetch_sub(aptr, val, memorder)  ({__auto_type rrres=*aptr; *aptr-=val; rrres;})
#define __atomic_fetch_add(aptr, val, memorder)  ({__auto_type rrres=*aptr; *aptr+=val; rrres;})
#define __atomic_fetch_and(aptr, val, memorder)  ({__auto_type rrres=*aptr; *aptr&=val; rrres;})
#else
#define __atomic_compare_exchange_n(aptr, aexpected, desired, weak, success_memorder, failure_memorder) (*aptr=desired,1)
#define __atomic_exchange_n(aptr, val, memorder) ({I rrres=(intptr_t)*aptr; *aptr=val; rrres;})
#define __atomic_fetch_or(aptr, val, memorder) ({I rrres=(intptr_t)*aptr; *aptr|=val; rrres;})
#define __atomic_fetch_sub(aptr, val, memorder) ({I rrres=(intptr_t)*aptr; *aptr-=val; rrres;})
#define __atomic_fetch_add(aptr, val, memorder) ({I rrres=(intptr_t)*aptr; *aptr+=val; rrres;})
#define __atomic_fetch_and(aptr, val, memorder) ({I rrres=(intptr_t)*aptr; *aptr&=val; rrres;})
#endif
#define __atomic_add_fetch(aptr, val, memorder) (*aptr+=val)
#define __atomic_sub_fetch(aptr, val, memorder) (*aptr-=val)
#define __atomic_and_fetch(aptr, val, memorder) (*aptr&=val)
#define REPATGCLIM 0   // no repat
#endif
//convenient abbreviations
#define casa(p,e,d) __atomic_compare_exchange_n(p,e,d,0,__ATOMIC_ACQ_REL,__ATOMIC_RELAXED)
#define cass(p,e,d) __atomic_compare_exchange_n(p,e,d,0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST)
#define aadd(p,v) __atomic_fetch_add(p,v,__ATOMIC_ACQ_REL)
#define adda(p,v) __atomic_add_fetch(p,v,__ATOMIC_ACQ_REL)
#define lda(p) __atomic_load_n(p,__ATOMIC_ACQUIRE)
#define lds(p) __atomic_load_n(p,__ATOMIC_SEQ_CST)
#define sta(p,v) __atomic_store_n(p,v,__ATOMIC_RELEASE) //technically not 'a'
#define sts(p,v) __atomic_store_n(p,v,__ATOMIC_SEQ_CST)
#define xchga(p,n) __atomic_exchange_n(p,n,__ATOMIC_ACQ_REL)

#ifndef SUPPORT_AFFINITY   // 1 iff the OS supports the pthreads call sched_setaffinity() and sched_getaffinity()
#if PYXES && !defined(__OpenBSD__)
#define SUPPORT_AFFINITY 1
#else
#define SUPPORT_AFFINITY 0
#endif
#endif


// Tuning options for cip.c
#define DCACHED_THRES  (64*64*64)    // when m*n*p less than this in a single thread use blocked; when higher, use cached
#if ((C_AVX2 || EMU_AVX2) && !PYXES)
// cachedmmult accuracy issue
#define DCACHED_THRESn  (10*10*10)    // when m*n*p less than this, don't even look for multithreads; use blocked
#else
#define DCACHED_THRESn  (24*24*24)    // when m*n*p less than this, don't even look for multithreads; use blocked
#endif
#if defined(__aarch64__) || defined(__APPLE__)
#define IGEMM_THRES  DCACHED_THRESn     // when m*n*p less than this use cached; when higher, use BLAS
#define DGEMM_THRES  DCACHED_THRESn     // when m*n*p less than this use cached; when higher, use BLAS   0 means 'always'
#define ZGEMM_THRES  DCACHED_THRESn     // when m*n*p less than this use cached; when higher, use BLAS   0 means 'always'
#elif ((C_AVX2 || EMU_AVX2) && PYXES) || !defined(_OPENMP)
#define IGEMM_THRES  (-1)     // when m*n*p less than this use cached; when higher, use BLAS
#define DGEMM_THRES  (-1)     // when m*n*p less than this use cached; when higher, use BLAS   _1 means 'never'
#define ZGEMM_THRES  (-1)     // when m*n*p less than this use cached; when higher, use BLAS   _1 means 'never'
#elif defined(_WIN32)
// tuned for windows
#if ((C_AVX2 || EMU_AVX2) && !PYXES)
#define IGEMM_THRES  DCACHED_THRESn     // when m*n*p less than this use cached; when higher, use BLAS
#define DGEMM_THRES  DCACHED_THRESn     // when m*n*p less than this use cached; when higher, use BLAS   0 means 'always'
#define ZGEMM_THRES  DCACHED_THRESn     // when m*n*p less than this use cached; when higher, use BLAS   0 means 'always'
#else
#define IGEMM_THRES  (400*400*400)   // when m*n*p less than this use cached; when higher, use BLAS
#define DGEMM_THRES  (300*300*300)   // when m*n*p less than this use cached; when higher, use BLAS   _1 means 'never'
#define ZGEMM_THRES  (400*400*400)   // when m*n*p less than this use cached; when higher, use BLAS  
#endif
#else
// tuned for linux
#if defined(_OPENMP) || ((C_AVX2 || EMU_AVX2) && !PYXES)
#define IGEMM_THRES  DCACHED_THRESn     // when m*n*p less than this use cached; when higher, use BLAS
#define DGEMM_THRES  DCACHED_THRESn     // when m*n*p less than this use cached; when higher, use BLAS   0 means 'always'
#define ZGEMM_THRES  DCACHED_THRESn     // when m*n*p less than this use cached; when higher, use BLAS   0 means 'always'
#else
#define IGEMM_THRES  (200*200*200)   // when m*n*p less than this use cached; when higher, use BLAS
#define DGEMM_THRES  (200*200*200)   // when m*n*p less than this use cached; when higher, use BLAS   _1 means 'never'
#define ZGEMM_THRES  (60*60*60)      // when m*n*p less than this use cached; when higher, use BLAS  
#endif
#endif

// prompt strings for jtgets()
#define GETSPROMPT "   "   // 3 spaces
#define GETSSUSPEND "      "   // 6 spaces in suspension
#define GETSKBD "\000\000"  // 00 - 1!:1]1 prompt from keyboard, which does some edits
#define GETSXDEF "\000\001"  // 01 - m : 0 suppress edits

#ifdef __x86_64__
#define FAST_AADD 1
#else
#define FAST_AADD 0
#endif

#define BETWEENC(x,lo,hi) ((UI)(((I)x)-(lo))<=(UI)((hi)-(lo)))   // x is in [lo,hi]
#define BETWEENO(x,lo,hi) ((UI)(((I)x)-(lo))<(UI)((hi)-(lo)))   // x is in [lo,hi)
#define BIT(n) ((I)1<<(n))    // I value with bit n set
#define BIT8(n) ((I8)1<<(n))    // I8 value with bit n set

// macros for bit testing
#define SGNIF(v,bitno) ((I)(v)<<(BW-1-(bitno)))  // Sets sign bit if the numbered bit is set
#define SGNIF4(v,bitno) ((I4)(v)<<(32-1-(bitno)))  // Sets sign bit if the numbered bit is set, in an I4
#define SGNONLYIF(v,bitno) (((v)>>(bitno))<<(BW-1))  // Sets sign bit if the numbered bit is set, clears all other bits
#define SGNIFNOT(v,bitno) (~SGNIF((v),(bitno)))  // Clears sign bit if the numbered bit is set
#define REPSGN(x) ((I)(x)>>(BW-1))  // replicate sign bit of x to entire word
#define REPSGN4(x) ((I4)(x)>>(32-1))  // replicate sign bit of x to entire I4 - x is forced to I4
#define REPSGNL(x) ((IL)(x)>>(64-1))  // replicate sign bit of x to entire IL
#define SGNTO0(x) ((UI)(x)>>(BW-1))  // move sign bit to bit 0, clear other bits
#define SGNTO0US(x) ((US)(x)>>(16-1))  // move sign bit to bit 0, clear other bits

#define ABS(a)          (0<=(a)?(a):-(a))
#define ACX(a)          {AC(a)=IMAX/2;}
#include "jr0.h" // moved to jr0.h
// #define ASSERT(b,e)     {if(!(b)){jsignal(e); R 0;}}
// #define ASSERTD(b,s)    {if(!(b)){jsigd((s)); R 0;}}
// #define ASSERTMTV(w)    {RZ(w); ASSERT(1==AR(w),EVRANK); ASSERT(!AN(w),EVLENGTH);}
// #define ASSERTN(b,e,nm) {if(!(b)){jt->curname=(nm); jsignal(e); R 0;}}
// #define ASSERTSYS(b,s)  {if(!(b)){jsignal(EVSYSTEM); jtwri(jt,MTYOSYS,"",(I)strlen(s),s); R 0;}}
// #define ASSERTW(b,e)    {if(!(b)){if((e)<=NEVM)jsignal(e); else jt->jerr=(e); R;}}
#define CALL1(f,w,fs)   ((f)(jt,    (w),(A)(fs)))
#define CALL2(f,a,w,fs) ((f)(jt,(a),(w),(A)(fs)))
#define DF1(f)          A f(J jt,    A w,A self)
#define DF2(f)          A f(J jt,A a,A w,A self)
#define DO(n,stm)       {I i=0,_n=(n); for(;i<_n;i++){stm}}
#define DQ(n,stm)       {I i=(n)-1;    for(;i>=0;--i){stm}}
#define ds(c)           pst[(UC)(c)]
#define EPILOG(z)       R gc(z,_ttop)
#define FDEPDEC(d)      {jt->fdepi-=d;}
#define FDEPINC(d)      {ASSERT(jt->fdepn>=d+jt->fdepi,EVSTACK); jt->fdepi+=d;}
#define FCONS(x)        fdef(CFCONS,VERB,jtnum1,jtnum2,0L,0L,(x),0L,RMAX,RMAX,RMAX)
#define FEQ(u,v)        (ABS((u)-(v))<=jt->fuzz*MAX(ABS(u),ABS(v)))
#define F1(f)           A f(J jt,    A w)
#define F2(f)           A f(J jt,A a,A w) 
#define F1RANK(m,f,self)    {RZ(   w); if(m<AR(w)         )R rank1ex(  w,(A)self,(I)m,     f);}
#define F2RANK(l,r,f,self)  {RZ(a&&w); if(l<AR(a)||r<AR(w))R rank2ex(a,w,(A)self,(I)l,(I)r,f);}
#define GA(v,t,n,r,s)   RZ(v=ga(t,(I)(n),(I)(r),(I*)(s)))
#define HN              3L
#define IC(w)           (AR(w) ? *AS(w) : 1L)
#define ICMP(z,w,n)     memcmp((z),(w),(n)*SZI)
#define ICPY(z,w,n)     memcpy((z),(w),(n)*SZI)
#define INF(x)          ((x)==inf||(x)==infm)
#define IX(n)           apv((n),0L,1L)
#define JATTN           {if(*jt->adbreak&&!jt->breakignore){jsignal(EVATTN); R 0;}}
#define JBREAK0         {if(2<=*jt->adbreak&&!jt->breakignore){jsignal(EVBREAK); R 0;}}
#define MAX(a,b)        ((a)>(b)?(a):(b))
#define MC              memcpy
#define MIN(a,b)        ((a)<(b)?(a):(b))
#define MLEN            (SY_64?63:31)
#define NAN0            (_clearfp())
#if (defined(__MSYS__) || (defined(MMSC_VER) && _MSC_VER==1800)) && !SY_64 // bug in some versions of VS 2013
#define NAN1            {if(_SW_INVALID&_statusfp()){_clearfp();jsignal(EVNAN); R 0;}}
#define NAN1V           {if(_SW_INVALID&_statusfp()){_clearfp();jsignal(EVNAN); R  ;}}
#define NANTEST         (_SW_INVALID&_statusfp())
#else
#define NAN1            {if(unlikely(_SW_INVALID&_clearfp())){jsignal(EVNAN); R 0;}}
#define NAN1V           {if(unlikely(_SW_INVALID&_clearfp())){jsignal(EVNAN); R  ;}}
#define NANTEST         (_SW_INVALID&_clearfp())

// for debug only
#ifdef _NAN
#undef NAN1
#undef NAN1V
#define NAN1            {if(_SW_INVALID&_clearfp()){fprintf(stderr,"nan error: file %s line %d\n",__FILE__,__LINE__);jsignal(EVNAN); R 0;}}
#define NAN1V           {if(_SW_INVALID&_clearfp()){fprintf(stderr,"nan error: file %s line %d\n",__FILE__,__LINE__);jsignal(EVNAN); R  ;}}
#endif
#endif
#define PROLOG          I _ttop=jt->tbase+jt->ttop
#define R               return
#define RE(exp)         {if((exp),jt->jerr)R 0;}
#define RER             {if(er){jt->jerr=er; R;}}
#define RESETERR        {jt->etxn=jt->jerr=0;}
#define RNE(exp)        {R jt->jerr?0:(exp);}
#define RZ(exp)         {if(!(exp))R 0;}
#define SBSV(x)         (jt->sbsv+(I)(x))
#define SBUV(x)         (jt->sbuv+(I)(x))
#define SGN(a)          ((0<(a))-(0>(a)))
#define SMAX            65535
#define SMIN            (-65536)
#define SZA             ((I)sizeof(A))
#define SZD             ((I)sizeof(D))
#define SZI             ((I)sizeof(I))
#define VAL1            '\001'
#define VAL2            '\002'


#if C_LE
#define B0000   0x00000000
#define B0001   0x01000000
#define B0010   0x00010000
#define B0011   0x01010000
#define B0100   0x00000100
#define B0101   0x01000100
#define B0110   0x00010100
#define B0111   0x01010100
#define B1000   0x00000001
#define B1001   0x01000001
#define B1010   0x00010001
#define B1011   0x01010001
#define B1100   0x00000101
#define B1101   0x01000101
#define B1110   0x00010101
#define B1111   0x01010101
#define BS00    0x0000
#define BS01    0x0100
#define BS10    0x0001
#define BS11    0x0101
#else
#define B0000   0x00000000
#define B0001   0x00000001
#define B0010   0x00000100
#define B0011   0x00000101
#define B0100   0x00010000
#define B0101   0x00010001
#define B0110   0x00010100
#define B0111   0x00010101
#define B1000   0x01000000
#define B1001   0x01000001
#define B1010   0x01000100
#define B1011   0x01000101
#define B1100   0x01010000
#define B1101   0x01010001
#define B1110   0x01010100
#define B1111   0x01010101
#define BS00    0x0000
#define BS01    0x0001
#define BS10    0x0100
#define BS11    0x0101
#endif

#if defined(__APPLE__) && defined(__aarch64__)
#define CACHELINESIZE 128  // size of processor cache line, in case we align to it
#else
#define CACHELINESIZE 64  // size of processor cache line, in case we align to it
#endif

#if !defined(C_CRC32C)
#if C_AVX2 ||  defined (__SSE4_2__) || defined(__aarch64__) || defined(_M_ARM64) || EMU_AVX2
#define C_CRC32C 1
#else
#define C_CRC32C 0
#endif

// JE source assumption of C_CRC32C
#if !(C_AVX2 || EMU_AVX2)
#undef C_CRC32C
#define C_CRC32C 0
#endif
#endif

// use viavx.c instead of vi.c
#if C_CRC32C && (C_AVX2 || EMU_AVX2)
#define C_VIAVX 1
#else
#define C_VIAVX 0
#endif

#define NORMAH1 0
#define NORMAH (7+NORMAH1)

#include "ja.h" 
#include "jc.h" 
#include "jtype.h" 
#include "jt.h" 
#include "jlib.h"
#include "je.h" 
#include "jerr.h" 
#include "va.h" 
#include "vq.h" 
#include "vx.h" 
#include "vz.h"
#include "vdx.h"  
#include "m.h"
#include "a.h"
#include "s.h"

// JPFX("here we are\n")
// JPF("size is %i\n", v)
// JPF("size and extra: %i %i\n", (v,x))
#define JPFX(s)  {char b[1000]; sprintf(b, s);    jsto(gjt,MTYOFM,b);}
#define JPF(s,v) {char b[1000]; sprintf(b, s, v); jsto(gjt,MTYOFM,b);}
extern J gjt; // global for JPF (procs without jt)

#if SY_WINCE_MIPS
/* strchr fails for CE MIPS - neg chars - spellit fails in ws.c for f=.+.  */
#define strchr(a,b)     (C*)strchr((unsigned char*)(a), (unsigned char)(b))
#endif
#if (defined(__arm__)||defined(__aarch64__)||defined(_M_ARM64)) && !defined(__APPLE__)
// option -fsigned-char in android and raspberry
#ifdef strchr
#undef strchr
#endif
#define strchr(a,b)     (C*)strchr((unsigned char*)(a), (unsigned char)(b))
#endif
#define ZZ(x)

#if defined(__wasm__)
#define FE_INVALID    1
#define __FE_DENORM   2
#define FE_DIVBYZERO  4
#define FE_OVERFLOW   8
#define FE_UNDERFLOW  16
#define FE_INEXACT    32
#endif

/* workaround clang branch prediction side effect */
#if defined(__clang__) && ( (__clang_major__ > 3) || ((__clang_major__ == 3) && (__clang_minor__ > 3)))
#define dmul2(u,v) ({__asm__("" ::: "cc");(u)*(v);})
#define ddiv2(u,v) ({__asm__("" ::: "cc");(u)/(v);})
#else
#define dmul2(u,v) ((u)*(v))
#define ddiv2(u,v) ((u)/(v))
#endif

/* (hopefully) turn off some re-scheduling optimization  */
#ifdef __GNUC__
#define CCBLOCK __asm__("" ::: "cc")
#else
#define CCBLOCK
#endif

#include <fenv.h>
#if SYS & SYS_UNIX
// bug clang isnan(x) set NaN flag if x is NaN
#if defined(ANDROID) && (defined(__aarch32__)||defined(__arm__))
#define _isnan       __builtin_isnan
#else
#define _isnan       isnan
#endif
#define _SW_INVALID  FE_INVALID

static inline UINT _clearfp(void){int r=fetestexcept(FE_ALL_EXCEPT);
 feclearexcept(FE_ALL_EXCEPT); return r;
}
#endif

// aligned memory allocation, assume align is power of 2
static INLINE void* aligned_malloc(size_t size, size_t align) {
 void *result;
 align = (align>=sizeof(void*))?align:sizeof(void*);
#ifdef _WIN32
 result = _aligned_malloc(size, align);
#elif ( !defined(ANDROID) || defined(__LP64__) )
/* posix_memalign does NOT set errno on failure; the error is returned */
 int err; 
 if((err=posix_memalign(&result, align, size))){ errno = err; result = 0;}
#else
 void *mem = malloc(size+(align-1)+sizeof(void*));
 if(mem){
  result = (void*)((uintptr_t)(mem+(align-1)+sizeof(void*)) & ~(align-1));
  ((void**)result)[-1] = mem;
 } else result = 0;
#endif
 return result;
}

static INLINE void aligned_free(void *ptr) {
#ifdef _WIN32
 _aligned_free(ptr);
#elif ( !defined(ANDROID) || defined(__LP64__) )
 free(ptr);
#else
 free(((void**)ptr)[-1]);
#endif
}

// Create (x&y) where x and y are signed, so we can test for overflow.
#if defined(MMSC_VER)  // SY_WIN32
#define XANDY(x,y) ((x)&(y))
#else
#define XANDY(x,y) ((I)((UI)(x)&(UI)(y)))
#endif

#ifdef _WIN32
#define FSYNC_STDERR
#else
#define FSYNC_STDERR fsync(STDERR_FILENO);
#endif
#define SEGFAULT        ({do{ \
                         fprintf(stderr,"trap : file %s line %d\n",__FILE__,__LINE__); \
                         FSYNC_STDERR; \
                         (void)__builtin_trap(); \
                        }while(0);0;})

// Supported in architecture ARMv8.1 and later
#if defined(__aarch64__) || defined(_M_ARM64)
  #define CRC32CW(crc, value) __asm__("crc32cw %w[c], %w[c], %w[v]":[c]"+r"(crc):[v]"r"(value))
  #define CRC32CX(crc, value) __asm__("crc32cx %w[c], %w[c], %x[v]":[c]"+r"(crc):[v]"r"(value))
  #define CRC32(crc,value)  ({ uint32_t crci=crc; CRC32CW(crci, value); crci; })
  #define CRC32L(crc,value) ({ uint64_t crci=crc; CRC32CX(crci, value); crci; })
  #define CRC32LL CRC32L                 // takes UIL (8 bytes), return UI

// The following definitions are used only in builds for the AVX instruction set
// 64-bit Atom cpu in android has hardware crc32c but not AVX
#elif defined(__SSE4_2__)
  #if defined(MMSC_VER)  // SY_WIN32
    // Visual Studio definitions
    #define CRC32(x,y) _mm_crc32_u32(x,y)  // takes UI4, returns UI4
    #define CRC32L(x,y) _mm_crc32_u64(x,y)  // takes UI, returns UI (top 32 bits 0)
  #else
    // gcc/clang definition
    #define CRC32(x,y) __builtin_ia32_crc32si(x,y)  // returns UI4
    #define CRC32L(x,y) __builtin_ia32_crc32di(x,y)  // returns UI
  #endif
  #define CRC32LL CRC32L                 // takes UIL (8 bytes), return UI

#else   // we use CRC32 for fast hashing; if not fast, we'll do it another way
#if defined(_WIN64)||defined(__LP64__)
  extern uint64_t crc32csb8(uint64_t crc, uint64_t value);
#else
  extern uint32_t crc32csb8(uint32_t crc, uint64_t value);
#endif
  extern uint32_t crc32csb4(uint32_t crc, uint32_t value);
  #define CRC32(x,y)  crc32csb4(x,y) // returns UI4
  #define CRC32L(x,y) crc32csb8(x,y) // returns UI
  #define CRC32LL CRC32L                 // takes UIL (8 bytes), return UI
#endif

