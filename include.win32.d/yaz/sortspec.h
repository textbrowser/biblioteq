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
 * \file sortspec.h
 * \brief Header for SortSpec parsing.
 */
#ifndef SORTSPEC_H
#define SORTSPEC_H

#include <yaz/yconfig.h>
#include <yaz/wrbuf.h>
#include <yaz/z-core.h>

YAZ_BEGIN_CDECL

/** \brief parse sort spec string
    \param odr memory for result
    \param arg sort spec string
    \returns Z39.50 SortKeySpecList or NULL on error

    The sort spec list is of the form:
    (field flags)+
    where field is either a string or one or more attribute pairs
    key=value (there must be no blanks in either field of flags).
    flags is a character list:
    dD>    : descending
    aA<    : ascending   (default)
    iI     : ignoreccase / case-insensitive (default)
    sS     : respectcase / case-sensitive
    !      : abort of key is omitted in result
    =value : missing value

    Examples:

      title a

      1=4 ia 1=1003 sd
*/
YAZ_EXPORT Z_SortKeySpecList *yaz_sort_spec(ODR odr, const char *arg);

/* \brief converts SortKeySpecList to CQL sortby string
   \param sksl SortKeySpecList
   \param w resulting CQL SORTBY string (of string to be appended)
   \retval 0 successful
   \retval -1 failure
*/
YAZ_EXPORT int yaz_sort_spec_to_cql(Z_SortKeySpecList *sksl, WRBUF w);

/* \brief adds PQF type-7 sorting to existing PQF from SortKeySpecList
   \param sksl SortKeySpecList
   \param w original PQF (without the Type-7)
   \retval 0 successful
   \retval -1 failure

   If successful, the pqf WRBUF holds the new PQF including the Type-7
   part.
*/
YAZ_EXPORT int yaz_sort_spec_to_type7(Z_SortKeySpecList *sksl, WRBUF pqf);

/* \brief converts Z39.50 SortKeySpecList to SRU 1.1 sort key
   \param sksl SortKeySpecList
   \param w resulting sortkey will be appended to this
   \retval 0 successful
   \retval -1 failure
*/
YAZ_EXPORT int yaz_sort_spec_to_srw_sortkeys(Z_SortKeySpecList *sksl, WRBUF w);

/* \brief converts SRU/SRW 1.1 sortkeys to Z39.50 sortspec (WRBUF)
   \param srw_sortkeys SRU sortkeys string
   \param w resulting Z39.50 sortspec, WRBUF string
   \retval 0 successful
   \retval -1 failure
*/
YAZ_EXPORT int yaz_srw_sortkeys_to_sort_spec(const char *srw_sortkeys, WRBUF w);

/* \brief converts Z39.50 SortKeySpecList to Solr sort key
   \param sksl SortKeySpecList
   \param w resulting sortkey will be appended to this
   \retval 0 successful
   \retval -1 failure
*/
YAZ_EXPORT int yaz_sort_spec_to_solr_sortkeys(Z_SortKeySpecList *sksl, WRBUF w);

/* \brief converts Solr sortkeys to Z39.50 sortspec (WRBUF)
   \param srw_sortkeys Solr sortkeys string
   \param w resulting Z39.50 sortspec, WRBUF string
   \retval 0 successful
   \retval -1 failure
*/
YAZ_EXPORT int yaz_solr_sortkeys_to_sort_spec(const char *srw_sortkeys, WRBUF w);

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

