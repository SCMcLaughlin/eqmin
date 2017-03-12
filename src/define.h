
#ifndef DEFINE_H
#define DEFINE_H

#if defined(_WIN32) || defined(WIN32)
# define PLATFORM_WINDOWS
#else
# define PLATFORM_UNIX
# define PLATFORM_LINUX
#endif

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

#ifdef PLATFORM_WINDOWS
# include <windows.h>
# include "win32_stdint.h"
# include "win32_posix_define.h"
#else
# include <stdint.h>
# include <stdatomic.h>
# include <errno.h>
# include <inttypes.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
#endif

#include "enum_err.h"

#ifdef PLATFORM_WINDOWS
# define API __declspec(dllexport)
#else
# define API extern
#endif

typedef uint8_t byte;
typedef int8_t bool;

#define true 1
#define false 0

#define KILOBYTES(n) (1024 * (n))
#define MEGABYTES(n) (1024 * KILOBYTES(n))

#define sizefield(type, name) sizeof(((type*)0)->name)

#endif/*DEFINE_H*/
