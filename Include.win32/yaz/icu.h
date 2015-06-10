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
    \brief ICU utilities
*/


#ifndef YAZ_ICU_H
#define YAZ_ICU_H

#include <yaz/yconfig.h>

#include <yaz/xmltypes.h>

#include <unicode/utypes.h>

YAZ_BEGIN_CDECL

/** \brief opaque ICU chain */
typedef struct icu_chain *yaz_icu_chain_t;

/** \brief destroys ICU chain */
YAZ_EXPORT void icu_chain_destroy(yaz_icu_chain_t chain);

/** \brief constructs ICU chain from XML specification
    \param xml_node icu_chain XML node - with attribute locale in it
    \param sort 1 if ICU chain is to deal with sort keys; 0 otherwise
    \param status May include ICU error code on failure
    \returns chain ptr or NULL on failure in which case status may hold info
*/
YAZ_EXPORT yaz_icu_chain_t icu_chain_xml_config(const xmlNode *xml_node,
                                                int sort,
                                                UErrorCode *status);
/** \brief pass string to ICU for parsing/tokenization/etc
    \param chain ICU chain to be used for parsing
    \param src8cstr input C string (null-terminated)
    \param status may include ICU error on failure
    \retval 0 failure
    \retval 1 success
*/
YAZ_EXPORT int icu_chain_assign_cstr(yaz_icu_chain_t chain,
                                     const char *src8cstr,
                                     UErrorCode *status);

/** \brief returns one token (if any)
    \param chain ICU chain
    \param status may include ICU error on failure
    \retval 0 error or end-of-tokens (no more tokens)
    \retval >0 token number (1, 3, 3, ..)

    This function tries to move to "next" token in assigned
    C-string .. Or returns 0 if no more is to be found
*/
YAZ_EXPORT int icu_chain_next_token(yaz_icu_chain_t chain,
                                    UErrorCode *status);

/** \brief returns token number of last token processed
    \brief chain ICU chain
    \returns token number (numbered from 1)
*/
YAZ_EXPORT int icu_chain_token_number(yaz_icu_chain_t chain);

/** \brief returns display token of last token processed
    \param chain ICU chain
    \returns display token string (C string)
    This function returns display string for last token returned
    by icu_chain_next_token.
*/
YAZ_EXPORT const char * icu_chain_token_display(yaz_icu_chain_t chain);

/** \brief returns normalized token of last token processed
    \param chain ICU chain
    \returns normalized token string (C string)
    This function returns normalized string for last token returned
    by icu_chain_next_token.
*/
YAZ_EXPORT const char * icu_chain_token_norm(yaz_icu_chain_t chain);

/** \brief returns sortkey token of last token processed
    \param chain ICU chain
    \returns sortkey token string (C string)
    This function returns sortkey string for last token returned
    by icu_chain_next_token.
*/
YAZ_EXPORT const char * icu_chain_token_sortkey(yaz_icu_chain_t chain);

/** \brief returns token as it relates to original text (legacy)
    \param chain ICU chain
    \param start offset in original text
    \param len number of uchars in original text
*/
YAZ_EXPORT void icu_chain_get_org_info(yaz_icu_chain_t chain,
                                       size_t *start, size_t *len);

/** \brief returns token as it relates to original text (2nd version)
    \param chain ICU chain
    \param start offset in original text
    \param len number of uchars in original text
    \param cstr if not-null, holds original string in there
*/
YAZ_EXPORT void icu_chain_get_org_info2(yaz_icu_chain_t chain,
                                        size_t *start, size_t *len,
                                        const char **cstr);

/** \brief ICU tokenizer iterator type (opaque) */
typedef struct icu_iter *yaz_icu_iter_t;

/** \brief create ICU tokenizer iterator from chain
    \param chain ICU chain
    \returns ICU iterator
*/
YAZ_EXPORT
yaz_icu_iter_t icu_iter_create(struct icu_chain *chain);

/** \brief starts iteration over string
    \param iter ICU tokenizer iterator
    \param src8cstr input string (0-terminated)

    Call icu_iter_next to iterate over each token.
*/
YAZ_EXPORT
void icu_iter_first(yaz_icu_iter_t iter, const char *src8cstr);

/** \brief iterates over one token
    \param iter ICU tokenizer iterator
    \retval 0 no more tokens (EOF)
    \retval 1 got one token (use icu_iter_get..-functions)
*/
YAZ_EXPORT
int icu_iter_next(yaz_icu_iter_t iter);

/** \brief destroy ICU tokenizer iterator
    \param iter ICU tokenizer iterator
*/
YAZ_EXPORT
void icu_iter_destroy(yaz_icu_iter_t iter);

/** \brief returns ICU normalized token
    \param iter ICU tokenizer iterator
    \returns string (0-terminated)
*/
YAZ_EXPORT
const char *icu_iter_get_norm(yaz_icu_iter_t iter);

/** \brief returns ICU sortkey string
    \param iter ICU tokenizer iterator
    \returns string (0-terminated)
*/
YAZ_EXPORT
const char *icu_iter_get_sortkey(yaz_icu_iter_t iter);

/** \brief returns ICU display string
    \param iter ICU tokenizer iterator
    \returns string (0-terminated)
*/
YAZ_EXPORT
const char *icu_iter_get_display(yaz_icu_iter_t iter);

/** \brief returns ICU token count for iterator
    \param iter ICU tokenizer iterator
    \returns token count (1, 2, 3...)
*/
YAZ_EXPORT
int icu_iter_get_token_number(yaz_icu_iter_t iter);

/** \brief returns ICU original token start (offset) and length (legacy)
    \param iter ICU tokenizer iterator
    \param start offset of last token in original text
    \param len length of last token in original text
*/
YAZ_EXPORT
void icu_iter_get_org_info(yaz_icu_iter_t iter, size_t *start, size_t *len);

/** \brief returns ICU original token start (offset) and length
    \param iter ICU tokenizer iterator
    \param start offset of last token in original text
    \param len length of last token in original text
    \param cstr if non-null: original string
*/
YAZ_EXPORT
void icu_iter_get_org_info2(yaz_icu_iter_t iter, size_t *start, size_t *len,
                            const char **cstr);

YAZ_END_CDECL

#endif /* YAZ_ICU_H */

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

