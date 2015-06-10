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
 * \file comstack.h
 * \brief Header for COMSTACK
 */

#ifndef COMSTACK_H
#define COMSTACK_H

#include <yaz/yconfig.h>
#include <yaz/oid_util.h>
#include <yaz/xmalloc.h>

YAZ_BEGIN_CDECL

struct comstack;
typedef struct comstack *COMSTACK;
typedef COMSTACK (*CS_TYPE)(int s, int flags, int protocol, void *vp);

struct comstack
{
    CS_TYPE type;
    int cerrno;     /* current error code of this stack */
    int iofile;    /* UNIX file descriptor for iochannel */
    void *cprivate;/* state info for lower stack */
    int max_recv_bytes;      /* max size of incoming package */
    int state;     /* current state */
#define CS_ST_UNBND      0
#define CS_ST_IDLE       1
#define CS_ST_INCON      2
#define CS_ST_OUTCON     3
#define CS_ST_DATAXFER   4
#define CS_ST_ACCEPT     5
#define CS_ST_CONNECTING 6
    int newfd;     /* storing new descriptor between listen and accept */
    int flags;     /* flags, blocking etc.. CS_FLAGS_..  */
    unsigned io_pending; /* flag to signal read / write op is incomplete */
    int event;     /* current event */
#define CS_NONE       0
#define CS_CONNECT    1
#define CS_DISCON     2
#define CS_LISTEN     3
#define CS_DATA       4
    enum oid_proto protocol;  /* what application protocol are we talking? */
    int (*f_put)(COMSTACK handle, char *buf, int size);
    int (*f_get)(COMSTACK handle, char **buf, int *bufsize);
    int (*f_more)(COMSTACK handle);
    int (*f_connect)(COMSTACK handle, void *address);
    int (*f_rcvconnect)(COMSTACK handle);
    int (*f_bind)(COMSTACK handle, void *address, int mode);
#define CS_CLIENT 0
#define CS_SERVER 1
    int (*f_listen)(COMSTACK h, char *raddr, int *addrlen,
                   int (*check_ip)(void *cd, const char *a, int len, int type),
                   void *cd);
    COMSTACK (*f_accept)(COMSTACK handle);
    void (*f_close)(COMSTACK handle);
    const char *(*f_addrstr)(COMSTACK handle);
    void *(*f_straddr)(COMSTACK handle, const char *str);
    int (*f_set_blocking)(COMSTACK handle, int blocking);
    void *user;       /* user defined data associated with COMSTACK */
};

#define cs_put(handle, buf, size) ((*(handle)->f_put)(handle, buf, size))
#define cs_get(handle, buf, size) ((*(handle)->f_get)(handle, buf, size))
#define cs_more(handle) ((*(handle)->f_more)(handle))
#define cs_connect(handle, address) ((*(handle)->f_connect)(handle, address))
#define cs_rcvconnect(handle) ((*(handle)->f_rcvconnect)(handle))
#define cs_bind(handle, ad, mo) ((*(handle)->f_bind)(handle, ad, mo))
#define cs_listen(handle, ap, al) ((*(handle)->f_listen)(handle, ap, al, 0, 0))
#define cs_listen_check(handle, ap, al, cf, cd) ((*(handle)->f_listen)(handle, ap, al, cf, cd))
#define cs_accept(handle) ((*(handle)->f_accept)(handle))
#define cs_close(handle) ((*(handle)->f_close)(handle))
#define cs_create(type, blocking, proto) ((*type)(-1, blocking, proto, 0))
#define cs_createbysocket(sock, type, blocking, proto) \
        ((*type)(sock, blocking, proto, 0))
#define cs_type(handle) ((handle)->type)
#define cs_fileno(handle) ((handle)->iofile)
#define cs_getstate(handle) ((handle)->getstate)
#define cs_errno(handle) ((handle)->cerrno)
#define cs_getproto(handle) ((handle)->protocol)
#define cs_addrstr(handle) ((*(handle)->f_addrstr)(handle))
#define cs_straddr(handle, str) ((*(handle)->f_straddr)(handle, str))
#define cs_want_read(handle) ((handle)->io_pending & CS_WANT_READ)
#define cs_want_write(handle) ((handle)->io_pending & CS_WANT_WRITE)
#define cs_set_blocking(handle,blocking) ((handle)->f_set_blocking(handle, blocking))

#define CS_WANT_READ 1
#define CS_WANT_WRITE 2

YAZ_EXPORT int cs_look (COMSTACK);
YAZ_EXPORT const char *cs_strerror(COMSTACK h);
YAZ_EXPORT const char *cs_errmsg(int n);
YAZ_EXPORT COMSTACK cs_create_host(const char *type_and_host,
                                   int blocking, void **vp);

YAZ_EXPORT COMSTACK cs_create_host_proxy(const char *vhost,
                                         int blocking, void **vp,
                                         const char *proxy_host);
YAZ_EXPORT COMSTACK cs_create_host2(const char *vhost, int blocking, void **vp,
                                    const char *proxy_host, int *proxy_mode);
YAZ_EXPORT void cs_get_host_args(const char *type_and_host, const char **args);
YAZ_EXPORT int cs_complete_auto_head(const char *buf, int len);
YAZ_EXPORT int cs_complete_auto(const char *buf, int len);
YAZ_EXPORT void *cs_get_ssl(COMSTACK cs)
#ifdef __GNUC__
    __attribute__ ((deprecated))
#endif
    ;
YAZ_EXPORT int cs_set_ssl_ctx(COMSTACK cs, void *ctx)
#ifdef __GNUC__
    __attribute__ ((deprecated))
#endif
    ;
YAZ_EXPORT int cs_set_ssl_certificate_file(COMSTACK cs, const char *fname);
YAZ_EXPORT int cs_get_peer_certificate_x509(COMSTACK cs, char **buf, int *len);
YAZ_EXPORT void cs_set_max_recv_bytes(COMSTACK cs, int max_recv_bytes);
YAZ_EXPORT void cs_print_session_info(COMSTACK cs);

YAZ_EXPORT int cs_parse_host(const char *uri, const char **host,
                             CS_TYPE *t, enum oid_proto *proto,
                             char **connect_host);

/*
 * error management.
 */

#define CSNONE     0
#define CSYSERR    1
#define CSOUTSTATE 2
#define CSNODATA   3
#define CSWRONGBUF 4
#define CSDENY     5
#define CSERRORSSL 6
#define CSBUFSIZE  7
#define CSLASTERROR CSBUFSIZE  /* must be the value of last CS error */

#define CS_FLAGS_BLOCKING 1
#define CS_FLAGS_NUMERICHOST 2
#define CS_FLAGS_DNS_NO_BLOCK 4

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

