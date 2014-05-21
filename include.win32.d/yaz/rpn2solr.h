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

/** \file
    \brief Header for RPN to SOLR tranform
*/

#ifndef RPN2SOLR_H_INCLUDED
#define RPN2SOLR_H_INCLUDED
#include <yaz/z-core.h>
#include <yaz/solr.h>
#include <yaz/wrbuf.h>

YAZ_BEGIN_CDECL

/** \brief transforms RPN query to SOLR output stream (re-entrant)
    \param ct SOLR transform handle
    \param addinfo additional info on error
    \param pr print function
    \param client_data opaque data to be passed to print handler
    \param q RPN Query
    \retval 0 success
    \retval !=0 failure (error code)
 */
YAZ_EXPORT
int solr_transform_rpn2solr_stream_r(solr_transform_t ct,
                                     WRBUF addinfo,
                                     void (*pr)(const char *buf, void *client_data),
                                     void *client_data,
                                     Z_RPNQuery *q);

/** \brief transforms RPN query to SOLR output stream (NOT re-entrant)
    \param ct SOLR transform handle
    \param pr print function
    \param client_data opaque data to be passed to print handler
    \param q RPN Query
    \retval 0 success
    \retval !=0 failure (error code)
 */
YAZ_EXPORT
int solr_transform_rpn2solr_stream(solr_transform_t ct,
                                 void (*pr)(const char *buf, void *client_data),
                                 void *client_data,
                                 Z_RPNQuery *q);


/** \brief transforms RPN query to SOLR WRBUF
    \param ct SOLR transform handle
    \param w WRBUF handle for result
    \param q RPN Query
    \retval 0 success
    \retval !=0 failure (error code)
 */
YAZ_EXPORT
int solr_transform_rpn2solr_wrbuf(solr_transform_t ct,
                                WRBUF w,
                                Z_RPNQuery *q);

/** \brief find a pattern that has a subset of attributes
    \param ct SOLR transform handle
    \param category pattern category
    \param attributes RPN attributes
    \returns pattern value or NULL if not found
 */
YAZ_EXPORT
const char *solr_lookup_reverse(solr_transform_t ct,
                               const char *category,
                               Z_AttributeList *attributes);

YAZ_END_CDECL

#endif
/* SOLR_H_INCLUDED */
/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

