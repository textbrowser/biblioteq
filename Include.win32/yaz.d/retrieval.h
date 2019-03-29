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
 * \file retrieval.h
 * \brief Retrieval utility
 */

#ifndef YAZ_RETRIEVAL_H
#define YAZ_RETRIEVAL_H

#include <stddef.h>
#include <yaz/wrbuf.h>
#include <yaz/yconfig.h>
#include <yaz/oid_util.h>
#include <yaz/record_conv.h>

YAZ_BEGIN_CDECL

/** retrieval handle  */
typedef struct yaz_retrieval_struct *yaz_retrieval_t;

/** creates retrieval handle
    \return retrieval handle
*/
YAZ_EXPORT yaz_retrieval_t yaz_retrieval_create(void);

/** destroys retrieval handle
    \param p retrieval handle
*/
YAZ_EXPORT void yaz_retrieval_destroy(yaz_retrieval_t p);

#if YAZ_HAVE_XML2
/** configures retrieval
    \param p retrieval handle
    \param node xmlNode pointer (root element of XML config)
    \retval 0 success
    \retval -1 failure

    On failure, call yaz_retrieval_get_error to get error string.

    For retrieval:
    \verbatim
     <retrievalinfo>
       <retrieval syntax="usmarc" name="F"/>
       <retrieval syntax="usmarc" name="B"/>
       <retrieval syntax="xml" name="marcxml"
		  identifier="info:srw/schema/1/marcxml-v1.1">
         <backend syntax="usmarc" name="F">
	   <marc inputformat="marc" outputformat="marcxml"
		 inputcharset="marc-8"/>
	 </backend>
       </retrieval>
       <retrieval syntax="xml" name="dc">
         <backend syntax="usmarc" name="F">
	   <marc inputformat="marc" outputformat="marcxml"
		 inputcharset="marc-8"/>
           <xslt stylesheet="MARC21slim2DC.xsl"/>
	 </backend>
       </retrieval>
     </retrievalinfo>
    \endverbatim
*/
YAZ_EXPORT
int yaz_retrieval_configure(yaz_retrieval_t p, const xmlNode *node);


/** configures retrieval with user-defined conversion types
    \param p retrieval handle
    \param node xmlNode pointer (root element of XML config)
    \param types record conversion types
    \retval 0 success
    \retval -1 failure

    On failure, use yaz_retrieval_get_error to get error string.
*/
YAZ_EXPORT
int yaz_retrieval_configure_t(yaz_retrieval_t p, const xmlNode *node,
                              struct yaz_record_conv_type *types);

#endif

/** performs retrieval request based on schema and format
    \param p retrieval handle
    \param schema record schema (SRU) / element set name (Z39.50)
    \param syntax record syntax (format)
    \param match_schema matched schema (if conversion was successful)
    \param match_syntax matced syntax OID  if conversion was successful)
    \param rc record conversion reference (if conversion was successful)
    \param backend_schema backend scchema (if conversion was successful)
    \param backend_syntax backend syntax (if conversion was successful)
    \retval 0 success, schema and syntax matches
    \retval -1 failure, use yaz_retrieval_get_error() for reason
    \retval 1 schema does not match
    \retval 2 syntax does not match
    \retval 3 both match but not together
*/
YAZ_EXPORT
int yaz_retrieval_request(yaz_retrieval_t p,
                          const char *schema, Odr_oid *syntax,
                          const char **match_schema, Odr_oid **match_syntax,
                          yaz_record_conv_t *rc,
                          const char **backend_schema,
                          Odr_oid **backend_syntax);

/** returns error string (for last error)
    \param p record conversion handle
    \return error string
*/
YAZ_EXPORT
const char *yaz_retrieval_get_error(yaz_retrieval_t p);


/** set path for opening stylesheets etc.
    \param p record conversion handle
    \param path file path (UNIX style with : / Windows with ;)
*/
YAZ_EXPORT
void yaz_retrieval_set_path(yaz_retrieval_t p, const char *path);

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

