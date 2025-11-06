/*
 * jansson_private_config.h
 *
 * Configuration file for Jansson private API
 * Generated for Linux/Unix systems
 */

#ifndef JANSSON_PRIVATE_CONFIG_H
#define JANSSON_PRIVATE_CONFIG_H

/* Include standard integer types */
#include <stdint.h>
#include <sys/types.h>

/* System headers availability */
#define HAVE_ENDIAN_H 1
#define HAVE_FCNTL_H 1
#define HAVE_SCHED_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_PARAM_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_STDINT_H 1

/* System functions availability */
#define HAVE_CLOSE 1
#define HAVE_GETPID 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_OPEN 1
#define HAVE_READ 1
#define HAVE_SCHED_YIELD 1

/* Compiler builtins */
#define HAVE_SYNC_BUILTINS 1
#define HAVE_ATOMIC_BUILTINS 1

/* Locale support */
#define HAVE_LOCALE_H 1
#define HAVE_SETLOCALE 1

/* Endianness - undef for little endian (x86/x64) */
/* #undef WORDS_BIGENDIAN */

/* Standard integer types */
#define HAVE_INT32_T 1
#define HAVE_UINT32_T 1
#define HAVE_UINT16_T 1
#define HAVE_UINT8_T 1
#define HAVE_SSIZE_T 1

/* Random number generation */
#define USE_URANDOM 1
/* #undef USE_WINDOWS_CRYPTOAPI */

/* Double to ASCII conversion */
/* #undef USE_DTOA */
#define DTOA_ENABLED 0

/* Initial hashtable order */
#define INITIAL_HASHTABLE_ORDER 3

#endif /* JANSSON_PRIVATE_CONFIG_H */
