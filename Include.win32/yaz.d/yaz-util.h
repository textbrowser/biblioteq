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
 * \file yaz-util.h
 * \brief Header for common YAZ utilities
 */

#ifndef YAZ_UTIL_H
#define YAZ_UTIL_H

#include <yaz/yconfig.h>
#include <yaz/yaz-version.h>
#include <yaz/xmalloc.h>

#include <yaz/log.h>

#include <yaz/tpath.h>
#include <yaz/options.h>
#include <yaz/wrbuf.h>
#include <yaz/nmem.h>
#include <yaz/readconf.h>
#include <yaz/marcdisp.h>
#include <yaz/yaz-iconv.h>
#include <yaz/matchstr.h>

/** \mainpage YAZ
    \section intro_sec Introduction

    For a general introduction to YAZ, read the
    YAZ User's Guide and Reference at
    http://www.indexdata.com/yaz/doc/
    This is also located in the doc directory of the YAZ distribution.
    (This main page is from yaz-util.h)

    The following sections are just a summary of the most important
    header files and where they belong.

    \section utilities Utilities
    Logging (syslog-like) utility \ref log.h

    Memory management for small blocks \ref nmem.h

    Write string buffer \ref wrbuf.h

    Options handling \ref options.h

    Character conversion \ref yaz-iconv.h

    MARC / MARCXML \ref marcdisp.h

    Testing framework: \ref test.h

    Record conversion: \ref record_conv.h

    Record retrieval: \ref retrieval.h

    Timing : \ref timing.h

    Locking: \ref mutex.h

    Daemon: \ref daemon.h

    Windows Service: \ref sc.h

    \section queryparsers Query parsers

    PQF parsing: \ref pquery.h

    CCL parsing: \ref ccl.h

    CQL parsing and conversion: \ref cql.h

    Z39.50 sort: \ref sortspec.h

    \section ber BER handling

    BER utilities (ODR): \ref odr.h

    \section z3950 Z39.50

    Z39.50 common header: \ref proto.h

    Z39.50 core codecs: \ref z-core.h.

    Z39.50 related formats:
    \ref z-accdes1.h ,  \ref z-core.h ,
    \ref z-estask.h , \ref z-oclcui.h , \ref z-sum.h
    \ref z-accform1.h , \ref z-date.h ,
    \ref z-exp.h ,    \ref z-opac.h ,   \ref z-sutrs.h
    \ref z-acckrb1.h ,  \ref z-diag1.h ,
    \ref z-grs.h ,    \ref z-rrf1.h ,   \ref z-uifr1.h
    \ref z-charneg.h ,  \ref z-espec1.h
    \ref z-mterm2.h , \ref z-rrf2.h ,   \ref z-univ.h

    Z39.50 extended services:
    \ref zes-admin.h , \ref zes-exps.h , \ref zes-pquery.h ,
    \ref zes-pset.h , \ref zes-update.h ,
    \ref zes-expi.h , \ref zes-order.h ,  \ref zes-psched.h ,
    \ref zes-update0.h

    Z39.50 diagnostics: \ref diagbib1.h

    Z39.50 externals: \ref prt-ext.h

    \section GDU Generic Data Unit (HTTP and BER)

    Definitions for GDU and HTTP: \ref zgdu.h

    \section SRU SRU

    SRU/SRW definitions: \ref srw.h

    SRW diagnostics: \ref diagsrw.h

    \section ILL ILL

    Common header: \ref ill.h

    Codecs: \ref ill-core.h

    \section ZOOM ZOOM

    Common header: \ref zoom.h

    \section GFS Generic Frontend Server (GFS)

    Header: \ref backend.h

*/
#endif

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

