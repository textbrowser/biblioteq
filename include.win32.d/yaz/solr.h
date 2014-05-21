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

/** \file solr.h
    \brief Header with public definitions about Solr.
*/

#ifndef SOLR_H_INCLUDED
#define SOLR_H_INCLUDED
#include <stdio.h>
#include <yaz/nmem.h>

YAZ_BEGIN_CDECL

/** \brief SOLR transform handle.
    The transform describes how to convert from SOLR to PQF (Type-1 AKA RPN).
*/
typedef struct cql_transform_t_ *solr_transform_t;

/** \brief creates a SOLR transform handle
    \returns transform handle or NULL for failure
*/
YAZ_EXPORT
solr_transform_t solr_transform_create(void);

/** \brief creates a SOLR transform handle from am opened file handle
    \param f file where transformation spec is read
    \returns transform handle or NULL for failure

    The transformation spec is read from a FILE handle which is assumed
    opened for reading.
*/
YAZ_EXPORT
solr_transform_t solr_transform_open_FILE(FILE *f);

/** \brief creates a SOLR transform handle from a file
    \param fname name of where transformation spec is read
    \returns transform handle or NULL for failure
*/
YAZ_EXPORT
solr_transform_t solr_transform_open_fname(const char *fname);


/** \brief defines SOLR transform pattern
    \param ct SOLR transform handle
    \param pattern pattern string
    \param value pattern value
    \returns 0 for succes; -1 for failure
*/
YAZ_EXPORT
int solr_transform_define_pattern(solr_transform_t ct, const char *pattern,
                                 const char *value);

/** \brief destroys a SOLR transform handle
    \param ct SOLR transform handle
 */
YAZ_EXPORT
void solr_transform_close(solr_transform_t ct);

/** \brief returns additional information for last transform
    \param ct SOLR transform handle
    \param addinfo additional info (result)
    \returns error code
 */
YAZ_EXPORT
int solr_transform_error(solr_transform_t ct, const char **addinfo);

/** \brief sets error and addinfo for transform
    \param ct SOLR transform handle
    \param error error code
    \param addinfo additional info
 */
YAZ_EXPORT
void solr_transform_set_error(solr_transform_t ct, int error, const char *addinfo);

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

