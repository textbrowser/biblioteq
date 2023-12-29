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
 * \file oid_util.h
 * \brief Header for OID basic functions
 */
#ifndef OID_UTIL_H
#define OID_UTIL_H

#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL

#define OID_SIZE 20
#define OID_STR_MAX 256

typedef short Odr_oid;   /* terminate by -1 */

typedef enum oid_proto
{
    PROTO_NOP=0,
    PROTO_Z3950,
    PROTO_HTTP
} oid_proto;


/** \brief copies OID
    \param t destination OID
    \param s source OID
*/
YAZ_EXPORT void oid_oidcpy(Odr_oid *t, const Odr_oid *s);

/** \brief appends to OID
    \param t destination OID
    \param s source OID
*/
YAZ_EXPORT void oid_oidcat(Odr_oid *t, const Odr_oid *s);


/** \brief compares OIDs
    \param o1 first OID
    \param o2 second OID
    \retval 0 equal
    \retval >0 o1 greater than o2
    \retval <0 o1 less than o2
*/
YAZ_EXPORT int oid_oidcmp(const Odr_oid *o1, const Odr_oid *o2);


/** \brief returns length of OIDs
    \param o OID
    \returns length as number of Odr_oids
*/
YAZ_EXPORT int oid_oidlen(const Odr_oid *o);

/** \brief converts OID to string (dot notation)
    \param oid OID
    \param oidbuf resulting buffer which should be at least of size OID_STR_MAX
    \returns result (same as oidbuf)
*/
YAZ_EXPORT char *oid_oid_to_dotstring(const Odr_oid *oid, char *oidbuf);


/** \brief converts dot string to OID
    \param name dot string OID, e.g. "1.2.840.10003.2.1"
    \param oid resulting OID buffer which should be at least of size OID_SIZE
    \retval 0 OK
    \retval -1 failure
*/
YAZ_EXPORT int oid_dotstring_to_oid(const char *name, Odr_oid *oid);

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

