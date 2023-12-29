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
 * \file mutex.h
 * \brief Header for Mutex functions
 */
#ifndef YAZ_MUTEX_H
#define YAZ_MUTEX_H

#include <stddef.h>
#include <time.h>
#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL

/** \brief YAZ MUTEX opaque pointer */
typedef struct yaz_mutex *YAZ_MUTEX;

/** \brief YAZ condition opaque pointer */
typedef struct yaz_cond *YAZ_COND;

/** \brief create MUTEX
    \param mutexp is pointer to MUTEX handle (*mutexp must be NULL)

    It is important that *mutexp is NULL. If not, yaz_mutex_create will
    not modify the handle (assumes it is already created!)
 */
YAZ_EXPORT void yaz_mutex_create(YAZ_MUTEX *mutexp);

/** \brief enter critical section / AKA lock
    \param mutex MUTEX handle
 */
YAZ_EXPORT void yaz_mutex_enter(YAZ_MUTEX mutex);


/** \brief leave critical section / AKA unlock
    \param mutex MUTEX handle
 */
YAZ_EXPORT void yaz_mutex_leave(YAZ_MUTEX mutex);


/** \brief destroy MUTEX
    \param mutexp pointer to MUTEX handle

    If *mutexp is NULL, then this function does nothing.
 */
YAZ_EXPORT void yaz_mutex_destroy(YAZ_MUTEX *mutexp);


/** \brief sets name of MUTEX for debugging purposes
    \param mutex MUTEX handle
    \param log_level YAZ log level
    \param name user-given name associated with MUTEX

    If log_level != 0 and name != 0 this function will make yaz_mutex_enter
    and yaz_mutex_leave print information for each invocation using yaz_log
    with the level given. In particular when YAZ is compiled with pthreads,
    yaz_mutex_enter will inform if a lock is not immediately acquired.
    This function should be called after a MUTEX is created but before
    it is used for locking.
 */
YAZ_EXPORT
void yaz_mutex_set_name(YAZ_MUTEX mutex, int log_level, const char *name);

/** \brief creates condition variable
    \param p reference to condition handle

    Upon successful completion *p holds the condition handle; *p = 0
    on error.
*/
YAZ_EXPORT void yaz_cond_create(YAZ_COND *p);

/** \brief destroys condition variable
    \param p reference to condition handle

    Upon completion *p holds 0.
*/
YAZ_EXPORT
void yaz_cond_destroy(YAZ_COND *p);

struct timeval;

/** \brief waits for condition
    \param p condition variable handle
    \param m mutex
    \param abstime wait until this time; 0 for indefinite wait

    Semantics like pthread_cond_wait.
*/
YAZ_EXPORT
int yaz_cond_wait(YAZ_COND p, YAZ_MUTEX m, const struct timeval *abstime);

/** \brief unblock one thread waiting for block
    \param p condition variable handle
*/
YAZ_EXPORT
int yaz_cond_signal(YAZ_COND p);

/** \brief unblock all threads waiting for block
    \param p condition variable handle
*/
YAZ_EXPORT
int yaz_cond_broadcast(YAZ_COND p);

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

