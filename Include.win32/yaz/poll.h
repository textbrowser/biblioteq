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
 * \file
 * \brief Poll, select wrappers
 */
#ifndef YAZ_POLL_H
#define YAZ_POLL_H

#include <stddef.h>
#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL

/** \brief select/poll masks .. timeout is "output" only */
enum yaz_poll_mask {
    yaz_poll_none = 0,
    yaz_poll_read = 1,
    yaz_poll_write = 2,
    yaz_poll_except = 4,
    yaz_poll_timeout = 8,
};

/** \brief select/poll fd info */
struct yaz_poll_fd {
    /** input mask passed to select/poll */
    enum yaz_poll_mask input_mask;
    /** resulting mask as returned by select/poll */
    enum yaz_poll_mask output_mask;
    /** file descriptor to use */
    int fd;
    /** user-defined data */
    void *client_data;
};

/** \brief poll wrapper for poll or select
    \param fds array of sockets to be polled
    \param num_fds size of fds array (number of sockets)
    \param sec number of seconds to wait (-1 for no timeout)
    \param nsec nano seconds to wait
    \retval -1 error
    \retval 0 timeout
    \retval >0 event occur on one or more sockets

    This functions calls yaz_poll_poll on systems where poll is
    available and yaz_poll_select where it it unavailable.
*/
YAZ_EXPORT
int yaz_poll(struct yaz_poll_fd *fds, int num_fds, int sec, int nsec);

#define yaz_poll_add(var,value) var = (enum yaz_poll_mask ) ((int) var | value)
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

