/** \file z-facet-1.h
    \brief ASN.1 Module UserInfoFormat-facet-1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_facet_1_H
#define z_facet_1_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_FacetList Z_FacetList;
YAZ_EXPORT int z_FacetList(ODR o, Z_FacetList **p, int opt, const char *name);

typedef struct Z_FacetField Z_FacetField;
YAZ_EXPORT int z_FacetField(ODR o, Z_FacetField **p, int opt, const char *name);

typedef struct Z_FacetTerm Z_FacetTerm;
YAZ_EXPORT int z_FacetTerm(ODR o, Z_FacetTerm **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_FacetList {
	int num;
	Z_FacetField **elements;
};

struct Z_FacetField {
	Z_AttributeList *attributes;
	int num_terms;
	Z_FacetTerm **terms; /* OPT */
};

struct Z_FacetTerm {
	Z_Term *term;
	Odr_int *count;
};

#ifdef __cplusplus
}
#endif
#endif
