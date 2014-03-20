/*
 * Sqltoast
 *
 * Copyright (C) 2010 Eric Day (eday@oddments.org) and Monty Taylor <mordred@inaugust.com>
 * All rights reserved.
 *
 * This code is almost verbatim from the Scale Stack project, which
 * is also licensed under the BSD license. See the
 * COPYING file in the root project directory for full text.
 */

/**
 * @file
 * @brief Common Macro Definitions
 */

#ifndef SQLTOAST_VISIBILITY_H
#define SQLTOAST_VISIBILITY_H

/**
 * Be sure to put SQLTOAST_API in front of all public API symbols, or one of
 * the other macros as appropriate. The default visibility without a macro is
 * to be hidden (SQLTOAST_LOCAL).
 */

#if defined(BUILDING_SQLTOAST) && defined(HAVE_VISIBILITY)
# if defined(__GNUC__)
#  define SQLTOAST_API __attribute__ ((visibility("default")))
#  define SQLTOAST_INTERNAL_API __attribute__ ((visibility("hidden")))
#  define SQLTOAST_API_DEPRECATED __attribute__ ((deprecated,visibility("default")))
#  define SQLTOAST_LOCAL  __attribute__ ((visibility("hidden")))
# elif (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550)) || (defined(__SUNPRO_CC) && (__SUNPRO_CC >= 0x550))
#  define SQLTOAST_API __global
#  define SQLTOAST_INTERNAL_API __hidden
#  define SQLTOAST_API_DEPRECATED __global
#  define SQLTOAST_LOCAL __hidden
# elif defined(_MSC_VER)
#  define SQLTOAST_API extern __declspec(dllexport)
#  define SQLTOAST_INTERNAL_API extern __declspec(dllexport)
#  define SQLTOAST_DEPRECATED_API extern __declspec(dllexport)
#  define SQLTOAST_LOCAL
# endif
#else  /* defined(BUILDING_SQLTOAST) && defined(HAVE_VISIBILITY) */
# if defined(_MSC_VER)
#  define SQLTOAST_API extern __declspec(dllimport)
#  define SQLTOAST_INTERNAL_API extern __declspec(dllimport)
#  define SQLTOAST_API_DEPRECATED extern __declspec(dllimport)
#  define SQLTOAST_LOCAL
# else
#  define SQLTOAST_API
#  define SQLTOAST_INTERNAL_API
#  define SQLTOAST_API_DEPRECATED
#  define SQLTOAST_LOCAL
# endif /* defined(_MSC_VER) */
#endif  /* defined(BUILDING_SQLTOAST) && defined(HAVE_VISIBILITY) */


#endif /* SQLTOAST_VISIBILITY_H */
