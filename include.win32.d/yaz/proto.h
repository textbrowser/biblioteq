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
 * \file proto.h
 * \brief Header for Z39.50 Protocol
 */
#ifndef Z_PROTO_H
#define Z_PROTO_H

#define Z_95 1
#define ASN_COMPILED 1

#include <yaz/yaz-version.h>
#include <yaz/z-accdes1.h>
#include <yaz/z-accform1.h>
#include <yaz/z-acckrb1.h>
#include <yaz/z-core.h>
#include <yaz/z-diag1.h>
#include <yaz/z-espec1.h>
#include <yaz/z-estask.h>
#include <yaz/z-exp.h>
#include <yaz/z-facet-1.h>
#include <yaz/z-grs.h>
#include <yaz/z-opac.h>
#include <yaz/z-rrf1.h>
#include <yaz/z-rrf2.h>
#include <yaz/z-sum.h>
#include <yaz/z-sutrs.h>
#include <yaz/z-uifr1.h>
#include <yaz/zes-expi.h>
#include <yaz/zes-exps.h>
#include <yaz/zes-order.h>
#include <yaz/zes-pquery.h>
#include <yaz/zes-psched.h>
#include <yaz/zes-pset.h>
#include <yaz/zes-update.h>
#include <yaz/zes-admin.h>
#include <yaz/z-date.h>
#include <yaz/z-univ.h>
#include <yaz/zes-update0.h>
#include <yaz/z-charneg.h>
#include <yaz/z-mterm2.h>
#include <yaz/wrbuf.h>
#include <yaz/matchstr.h>
#include <yaz/zgdu.h>
#include <yaz/z-oclcui.h>

#include <yaz/marcdisp.h>
YAZ_BEGIN_CDECL

/* Here are a lot of definitions that were defined in YAZ 1 and which
   are still in use by some applications. We keep those to preserve
   backwards compatibility.. They cause no harm.
*/
#define Z_PRES_SUCCESS   Z_PresentStatus_success
#define Z_PRES_PARTIAL_1 Z_PresentStatus_partial_1
#define Z_PRES_PARTIAL_2 Z_PresentStatus_partial_2
#define Z_PRES_PARTIAL_3 Z_PresentStatus_partial_3
#define Z_PRES_PARTIAL_4 Z_PresentStatus_partial_4
#define Z_PRES_FAILURE   Z_PresentStatus_failure

#define Z_RES_SUBSET  Z_SearchResponse_subset
#define Z_RES_INTERIM Z_SearchResponse_interim
#define Z_RES_NONE    Z_SearchResponse_none

#define Z_SortStatus_success Z_SortResponse_success
#define Z_SortStatus_partial_1 Z_SortResponse_partial_1
#define Z_SortStatus_failure Z_SortResponse_failure

#define Z_SortRelation_ascending            Z_SortKeySpec_ascending
#define Z_SortRelation_descending           Z_SortKeySpec_descending
#define Z_SortRelation_ascendingByFreq      Z_SortKeySpec_ascendingByFrequency
#define Z_SortRelation_descendingByFreq     Z_SortKeySpec_descendingByfrequency

#define Z_SortCase_caseSensitive            Z_SortKeySpec_caseSensitive
#define Z_SortCase_caseInsensitive          Z_SortKeySpec_caseInsensitive

#define Z_TriggerResourceCtrl_resourceReport Z_TriggerResourceControlRequest_resourceReport
#define Z_TriggerResourceCtrl_resourceControl  Z_TriggerResourceControlRequest_resourceControl
#define Z_TriggerResourceCtrl_cancel Z_TriggerResourceControlRequest_cancel

#define Z_DeleteRequest_list    Z_DeleteResultSetRequest_list
#define Z_DeleteRequest_all     Z_DeleteResultSetRequest_all

#define Z_AccessRequest_simpleForm Z_AccessControlRequest_simpleForm
#define Z_AccessRequest_externallyDefined Z_AccessControlRequest_externallyDefined

#define Z_AccessResponse_simpleForm Z_AccessControlResponse_simpleForm
#define Z_AccessResponse_externallyDefined Z_AccessControlResponse_externallyDefined

#define Z_ResourceReportStatus_success   Z_ResourceReportResponse_success
#define Z_ResourceReportStatus_partial   Z_ResourceReportResponse_partial
#define Z_ResourceReportStatus_failure_1 Z_ResourceReportResponse_failure_1
#define Z_ResourceReportStatus_failure_2 Z_ResourceReportResponse_failure_2
#define Z_ResourceReportStatus_failure_3 Z_ResourceReportResponse_failure_3
#define Z_ResourceReportStatus_failure_4 Z_ResourceReportResponse_failure_4
#define Z_ResourceReportStatus_failure_5 Z_ResourceReportResponse_failure_5
#define Z_ResourceReportStatus_failure_6 Z_ResourceReportResponse_failure_6

#define Z_SortResultSetStatus_empty       Z_SortResponse_empty
#define Z_SortResultSetStatus_interim     Z_SortResponse_interim
#define Z_SortResultSetStatus_unchanged   Z_SortResponse_unchanged
#define Z_SortResultSetStatus_none        Z_SortResponse_none

typedef Z_External Z_DatabaseRecord;
typedef struct Z_IOItemOrder Z_ItemOrder;

YAZ_EXPORT Z_APDU *zget_APDU(ODR o, int which);

/** \brief Performs "pretty" display of GRS-1 record to WRBUF */
YAZ_EXPORT void yaz_display_grs1(WRBUF wrbuf, Z_GenericRecord *r, int flags);

/** \brief Encodes Z39.50 Init OPtions based on string mnemonics */
YAZ_EXPORT int yaz_init_opt_encode(Z_Options *opt, const char *opt_str,
                                   int *error_pos);

/** \brief Decodes Z39.50 Init Options - for printing */
YAZ_EXPORT void yaz_init_opt_decode(Z_Options *opt,
                                    void (*pr)(const char *name,
                                               void *clientData),
                                    void *clientData);
/** \brief Creates Default Diag Format Diagnostic */
YAZ_EXPORT
Z_DefaultDiagFormat *zget_DefaultDiagFormat(ODR o, int error,
                                            const char *addinfo);

/** \brief Creates Surrogate Diagnostic Records */
YAZ_EXPORT
Z_NamePlusRecord *zget_surrogateDiagRec(ODR o, const char *dbname,
                                        int error, const char *addinfo);

/** \brief Creates Initialize Response diagnostics */
YAZ_EXPORT
Z_External *zget_init_diagnostics(ODR odr, int error, const char *addinfo);

/** \brief Creates Initialize Response diagnostics (Octet-aligned EXTERNAL) */
YAZ_EXPORT
Z_External *zget_init_diagnostics_octet(ODR odr, int error, const char *addinfo);

/** \brief Creates Diagnostic record - Z_DiagRecs type */
YAZ_EXPORT
Z_DiagRecs *zget_DiagRecs(ODR o, int error, const char *addinfo);

/** \brief Creates Diagnostic record - Z_DiagRecs type */
YAZ_EXPORT
Z_DiagRec *zget_DiagRec(ODR o, int error, const char *addinfo);

/** \brief get element set name from RecordComposition
    \param comp record composition
    \returns element set name or NULL if no element set name was given
*/
YAZ_EXPORT
const char *yaz_get_esn(Z_RecordComposition *comp);

/** \brief set element set name in RecordComposition struct
    \param comp_p record composition ptr
    \param esn element set name string
    \param nmem memory for result composition
*/
YAZ_EXPORT
void yaz_set_esn(Z_RecordComposition **comp_p, const char *esn, NMEM nmem);

/** \brief returns diagnostics from an init response
    \param no number of diagnotic entry to get. 0=first, 1=second, ..
    \param initrs InitResponse structure
    \returns diag entry or 0 (NULL) if none could be found
*/
YAZ_EXPORT
Z_DefaultDiagFormat *yaz_decode_init_diag(int no, Z_InitResponse *initrs);

/** \brief creates AttributeList with type=1(use) and string value
    \param o ODR for encoding
    \param name use-value
    \returns attribute list with only one use-attribute
*/
YAZ_EXPORT
Z_AttributeList *zget_AttributeList_use_string(ODR o, const char *name);

YAZ_END_CDECL

#include <yaz/prt-ext.h>

#endif
/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

