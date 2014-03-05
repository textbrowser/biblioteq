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
 * \file facet.h
 * \brief Header for the facet utilities
 */

#ifndef YAZ_FACET_H
#define YAZ_FACET_H

#include <yaz/yconfig.h>
#include <yaz/odr.h>
#include <yaz/z-core.h>
#include <yaz/z-facet-1.h>
#include <yaz/log.h>

YAZ_BEGIN_CDECL

/*
 * Helper function for extracting facet values from the ASN structures.
 *
 */

/** A helper structure to extract all the attribute stuff
   from one Z_AttributesList for facets. The pointers will all be to
   the Z-structures, or to constants, so there is no need to
   worry about freeing them */
struct yaz_facet_attr {
    int  errcode;   /* set in case of errors */
    char *errstring; /* opt */
    const char *useattr; /* @attr 1, from a string attr */
                   /* or number converted to a string */
                   /* defaults to 'any' */
    char useattrbuff[30]; /* for converting numbers to strings */
    int sortorder;
    int limit;
    int start;
};

YAZ_EXPORT
void yaz_facet_attr_init(struct yaz_facet_attr *attr_values);

YAZ_EXPORT
void yaz_facet_attr_get_z_attributes(const Z_AttributeList *attributes,
				     struct yaz_facet_attr *av);

YAZ_EXPORT
Z_FacetTerm *facet_term_create_cstr(ODR odr, const char *cstr, Odr_int freq);

YAZ_EXPORT
Z_FacetField* facet_field_create(ODR odr, Z_AttributeList *attributes,
				 int num_terms);

YAZ_EXPORT
void facet_field_term_set(ODR odr, Z_FacetField *field,
			  Z_FacetTerm *facetTerm, int index);

YAZ_EXPORT
Z_FacetList* facet_list_create(ODR odr, int num_facets);

YAZ_EXPORT
void facet_list_field_set(ODR odr, Z_FacetList *list, Z_FacetField *field,
			  int index);
YAZ_EXPORT
void yaz_oi_set_facetlist(
    Z_OtherInformation **otherInformation, ODR odr, Z_FacetList *facet_list);

YAZ_EXPORT
Z_FacetList *yaz_oi_get_facetlist(Z_OtherInformation **otherInformation);

YAZ_EXPORT
void yaz_oi_set_facetlist(Z_OtherInformation **otherInformation, ODR odr,
                          Z_FacetList *facet_list);


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

