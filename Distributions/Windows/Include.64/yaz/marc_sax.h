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
 * \file marc_sax.h
 * \brief Parsing MARCXML collection using Libxml2's SAX parser.
 */

#ifndef MARC_SAX_H
#define MARC_SAX_H

#include <yaz/yconfig.h>
#include <yaz/marcdisp.h>

#if YAZ_HAVE_XML2
#include <libxml/parser.h>
#endif

YAZ_BEGIN_CDECL

typedef struct yaz_marc_sax_t_ *yaz_marc_sax_t;

#if YAZ_HAVE_XML2

/** \brief construct marc SAX parser
    \param mt marc handle
    \param cb function called for each record
    \param cb_data data to be passed to each cb call
  */
YAZ_EXPORT yaz_marc_sax_t yaz_marc_sax_new(yaz_marc_t mt,
    void (*cb)(yaz_marc_t mt, void *), void *cb_data);

/** \brief return Libxml SAX handler pointer
    \returns pointer
  */
YAZ_EXPORT xmlSAXHandlerPtr yaz_marc_sax_get_handler(yaz_marc_sax_t ctx);

/** \brief destroys marc SAX parser
    \param ctx
  */
YAZ_EXPORT void yaz_marc_sax_destroy(yaz_marc_sax_t ctx);

#endif

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

