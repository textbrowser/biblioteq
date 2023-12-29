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
 * \file daemon.h
 * \brief Unix daemon management
 */

#ifndef YAZ_DAEMON_H
#define YAZ_DAEMON_H

#include <stdio.h>
#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL

#define YAZ_DAEMON_FORK 1
#define YAZ_DAEMON_DEBUG 2
#define YAZ_DAEMON_KEEPALIVE 4
#define YAZ_DAEMON_LOG_REOPEN 8

/** \brief daemon utility.
    \param progname program name for logging purposes.
    \param flags flags which is a bit-wise combination of YAZ_DAEMON_..
    \param work working handler (which may be running in different process)
    \param data opaque data to be passed to work handler
    \param pidfile filename with Process-ID (NULL for no file)
    \param uid effective user ID for handler (NULL for no same as caller)
    \returns 0 for success, non-zero for failure.

    This function puts calls work handler which is supposed to carry
    out a daemon service with a possible changed User ID and in a
    child process.

    Flag YAZ_DAEMON_FORK: Puts the service in the background on Unix.

    Flag YAZ_DAEMON_DEBUG: Puts the service in debug mode (no fork at all).

    Flag YAZ_DAEMON_KEEPALIVE: Repeatedly calls work handler if it makes a
    "fatal" error.

    Flag YAZ_DAEMON_LOG_REOPEN: Re-opens yaz log if SIGHUP is received
*/
YAZ_EXPORT
int yaz_daemon(const char *progname,
               unsigned int flags,
               void (*work)(void *data), void *data,
               const char *pidfile, const char *uid);

/** \brief stop daemon - stop parent process

    This function sends a signal to the parent keepalive process that
    makes it exit immediately - without waiting. If there's no parent
    keepalive process, this function does nothing. SHould be called
    when the child process has freed resources, such as listening socket.
    But the child process may continue running.
*/
YAZ_EXPORT
void yaz_daemon_stop(void);

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

