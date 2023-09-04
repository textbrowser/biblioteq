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
 * \file copy_types.h
 * \brief Copies various Z39.50 types
 */

#ifndef YAZ_COPY_TYPES_H
#define YAZ_COPY_TYPES_H

#include <yaz/yconfig.h>
#include <yaz/proto.h>

YAZ_BEGIN_CDECL

YAZ_EXPORT
Z_RPNQuery *yaz_copy_z_RPNQuery(Z_RPNQuery *q, ODR out);

YAZ_EXPORT
Z_Query *yaz_copy_Z_Query(Z_Query *q, ODR out);

YAZ_EXPORT
Z_RPNQuery *yaz_clone_z_RPNQuery(Z_RPNQuery *q, NMEM out);

YAZ_EXPORT
int yaz_compare_z_RPNQuery(Z_RPNQuery *a, Z_RPNQuery *b);

YAZ_EXPORT
Z_Query *yaz_clone_z_Query(Z_Query *q, NMEM out);

YAZ_EXPORT
int yaz_compare_z_Query(Z_Query *a, Z_Query *b);

YAZ_EXPORT
Z_NamePlusRecord *yaz_clone_z_NamePlusRecord(Z_NamePlusRecord *s, NMEM out);
YAZ_EXPORT
int yaz_compare_z_NamePlusRecord(Z_NamePlusRecord *a, Z_NamePlusRecord *b);

YAZ_EXPORT
Z_RecordComposition *yaz_clone_z_RecordComposition(Z_RecordComposition *s,
                                                   NMEM out);
YAZ_EXPORT
int yaz_compare_z_RecordComposition(Z_RecordComposition *a,
                                    Z_RecordComposition *b);

YAZ_EXPORT
Z_OtherInformation *yaz_clone_z_OtherInformation(Z_OtherInformation *s,
                                                 NMEM out);

YAZ_EXPORT
int yaz_compare_z_OtherInformation(Z_OtherInformation *a,
                                   Z_OtherInformation *b);

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

