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

/**
 * \file timing.h
   \brief Timing utilities

 */

#ifndef YAZ_TIMING_H
#define YAZ_TIMING_H

#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL

/** \brief YAZ timing handle (opaque type)
 */
typedef struct yaz_timing *yaz_timing_t;

/** \brief create timing handle
    \returns timing handle

   \verbatim
    yaz_timing_t my = yaz_timing_create();
    yaz_timing_start(my);
    do_work();
    yaz_timing_stop(my);

    printf("real=%g user=%g sys=%g\n",
       yaz_timing_get_real(my),
       yaz_timing_get_user(my),
       yaz_timing_get_sys(my));

    yaz_timing_destroy(my);

   \endverbatim
 */
YAZ_EXPORT
yaz_timing_t yaz_timing_create(void);

/** \brief start timer
    \param t timing handle
 */
YAZ_EXPORT
void yaz_timing_start(yaz_timing_t t);

/** \brief stop timer
    \param t timing handle

    Use yaz_timing_get_real, yaz_timing_get_user and yaz_timing_get_sys
    to fetch timing results.
*/
YAZ_EXPORT
void yaz_timing_stop(yaz_timing_t t);

/** \brief returns real time in seconds
    \param t timing handle
    \returns time in seconds
*/
YAZ_EXPORT
double yaz_timing_get_real(yaz_timing_t t);

/** \brief returns user time in seconds
    \param t timing handle
    \returns time in seconds
*/
YAZ_EXPORT
double yaz_timing_get_user(yaz_timing_t t);

/** \brief returns system time in seconds
    \param t timing handle
    \returns time in seconds
*/
YAZ_EXPORT
double yaz_timing_get_sys(yaz_timing_t t);

/** \brief destroys timing handle
*/
YAZ_EXPORT
void yaz_timing_destroy(yaz_timing_t *tp);

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

