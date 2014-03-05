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
 * \file charneg.h
 * \brief Header for Z39.50 Charset negotiation utilities
 *
 * Helper functions for Character Set and Language Negotiation - 3
 */
#ifndef CHARNEG_H
#define CHARNEG_H

#include <yaz/proto.h>

YAZ_BEGIN_CDECL

/*
 * Character set and language negotiation model - 3 (*)
 *
 * These a set helper functions for support (*). There are
 * client and server parts. At that moment the (*) is based on
 * Z_PrivateCharacterSet_externallySpecified method.
 *
 */

/*
 * Get (*) record from request
 */
YAZ_EXPORT
Z_CharSetandLanguageNegotiation *yaz_get_charneg_record(Z_OtherInformation *p);

/*
 * Delete (*) record from request
 */
YAZ_EXPORT
int yaz_del_charneg_record(Z_OtherInformation **p);

/*
 * Client's part.
 */
YAZ_EXPORT
Z_External *yaz_set_proposal_charneg(ODR odr,
                                     const char **charsets, int num_charsets,
                                     const char **langs,
                                     int num_langs, int selected);

YAZ_EXPORT
Z_External *yaz_set_proposal_charneg_list(ODR o,
                                          const char *delim,
                                          const char *charset_list,
                                          const char *lang_list,
                                          int selected);

YAZ_EXPORT
void yaz_get_response_charneg(NMEM mem,
                              Z_CharSetandLanguageNegotiation *p,
                              char **charset, char **lang,
                              int *selected);

/*
 * Server's part
 */

YAZ_EXPORT
Z_External *yaz_set_response_charneg(ODR odr,
                                     const char *charset, const char *lang,
                                     int selected);


YAZ_EXPORT
void yaz_get_proposal_charneg(NMEM mem,
                              Z_CharSetandLanguageNegotiation *p,
                              char ***charsets, int *num_charsets,
                              char ***langs, int *num_langs,
                              int *selected);


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

