/* This file is part of the YAZ toolkit.
 * Copyright (C) Index Data.
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Index Data nor the names of its contributors
 *       may be used to endorse or promote products derived from this
 *       software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file test.h
    \brief Unit Test for YAZ
*/

#ifndef YAZ_TEST_H
#define YAZ_TEST_H

#include <yaz/yconfig.h>
#include <stdio.h>

/** \brief Get the verbosity level */
int yaz_test_get_verbosity(void);

/** \brief Test OK */
#define YAZ_TEST_TYPE_OK 1
/** \brief Test failed */
#define YAZ_TEST_TYPE_FAIL 2

/** \brief boolean test. as only evaluated once */
#define YAZ_CHECK(as) { \
  if (as) { \
    yaz_check_print1(YAZ_TEST_TYPE_OK, __FILE__, __LINE__, #as); \
  } else { \
    yaz_check_print1(YAZ_TEST_TYPE_FAIL, __FILE__, __LINE__, #as); \
  } \
}

/** \brief a test we know will fail at this time.
 *
 * Later, when the bug is fixed, this test will suddenly pass,
 * which will be reported as an error, to remind you to go and fix
 * your tests.
 */

#define YAZ_CHECK_TODO(as) { \
  yaz_check_inc_todo(); \
  if (!as) { \
    yaz_check_print1(YAZ_TEST_TYPE_OK, __FILE__, __LINE__, "TODO: " #as); \
  } else { \
    yaz_check_print1(YAZ_TEST_TYPE_FAIL, __FILE__, __LINE__, "TODO: "#as); \
  } \
}

/** \brief equality test. left, right only evaluated once */
#define YAZ_CHECK_EQ(left, right) { \
  int lval = left; \
  int rval = right; \
  if (lval == rval) { \
    yaz_check_eq1(YAZ_TEST_TYPE_OK, __FILE__, __LINE__, \
     #left, #right, lval, rval); \
  } else { \
    yaz_check_eq1(YAZ_TEST_TYPE_FAIL, __FILE__, __LINE__, \
     #left, #right, lval, rval); \
  } \
}

/** \brief Macro to initialize the system (in start of main typically) */
#define YAZ_CHECK_INIT(argc, argv) yaz_check_init1(&argc, &argv)
/** \brief Macro to terminate the system (end of main, normally) */
#define YAZ_CHECK_TERM yaz_check_term1(); return 0

/** \brief Macro to enable and initialize the yaz_log(start of main) */
#define YAZ_CHECK_LOG() yaz_check_init_log(argv[0])

YAZ_BEGIN_CDECL

/** \brief used by macro. Should not be called directly */
YAZ_EXPORT void yaz_check_init1(int *argc, char ***argv);

/** \brief used by macro. Should not be called directly */
YAZ_EXPORT void yaz_check_term1(void);

/** \brief used by macro. Should not be called directly */
YAZ_EXPORT void yaz_check_init_log(const char *argv0);

/** \brief used by macro. Should not be called directly */
YAZ_EXPORT void yaz_check_print1(int type, const char *file, int line,
                                 const char *expr);
/** \brief used by macro. Should not be called directly */
YAZ_EXPORT void yaz_check_eq1(int type, const char *file, int line,
                              const char *left, const char *right,
                              int lval, int rval);
/** \brief used by macro. Should not be called directly */
YAZ_EXPORT void  yaz_check_inc_todo(void);
YAZ_END_CDECL

#endif
/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

