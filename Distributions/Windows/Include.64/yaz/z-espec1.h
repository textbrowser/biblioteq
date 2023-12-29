/** \file z-espec1.h
    \brief ASN.1 Module ElementSpecificationFormat-eSpec-1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_espec1_H
#define z_espec1_H

#include <yaz/odr.h>
#include <yaz/z-grs.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_Espec1 Z_Espec1;
YAZ_EXPORT int z_Espec1(ODR o, Z_Espec1 **p, int opt, const char *name);

typedef struct Z_ElementRequestCompositeElementPrimitives Z_ElementRequestCompositeElementPrimitives;
YAZ_EXPORT int z_ElementRequestCompositeElementPrimitives(ODR o, Z_ElementRequestCompositeElementPrimitives **p, int opt, const char *name);

typedef struct Z_ElementRequestCompositeElementSpecs Z_ElementRequestCompositeElementSpecs;
YAZ_EXPORT int z_ElementRequestCompositeElementSpecs(ODR o, Z_ElementRequestCompositeElementSpecs **p, int opt, const char *name);

typedef struct Z_ElementRequestCompositeElement Z_ElementRequestCompositeElement;
YAZ_EXPORT int z_ElementRequestCompositeElement(ODR o, Z_ElementRequestCompositeElement **p, int opt, const char *name);

typedef struct Z_ElementRequest Z_ElementRequest;
YAZ_EXPORT int z_ElementRequest(ODR o, Z_ElementRequest **p, int opt, const char *name);

typedef struct Z_SimpleElement Z_SimpleElement;
YAZ_EXPORT int z_SimpleElement(ODR o, Z_SimpleElement **p, int opt, const char *name);

typedef struct Z_SpecificTag Z_SpecificTag;
YAZ_EXPORT int z_SpecificTag(ODR o, Z_SpecificTag **p, int opt, const char *name);

typedef struct Z_ETagUnit Z_ETagUnit;
YAZ_EXPORT int z_ETagUnit(ODR o, Z_ETagUnit **p, int opt, const char *name);

typedef struct Z_ETagPath Z_ETagPath;
YAZ_EXPORT int z_ETagPath(ODR o, Z_ETagPath **p, int opt, const char *name);

typedef struct Z_OccurValues Z_OccurValues;
YAZ_EXPORT int z_OccurValues(ODR o, Z_OccurValues **p, int opt, const char *name);

typedef struct Z_Occurrences Z_Occurrences;
YAZ_EXPORT int z_Occurrences(ODR o, Z_Occurrences **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_Espec1 {
	int num_elementSetNames;
	Z_InternationalString **elementSetNames; /* OPT */
	Odr_oid *defaultVariantSetId; /* OPT */
	Z_Variant *defaultVariantRequest; /* OPT */
	Odr_int *defaultTagType; /* OPT */
	int num_elements;
	Z_ElementRequest **elements; /* OPT */
};

struct Z_ElementRequestCompositeElementPrimitives {
	int num;
	Z_InternationalString **elements;
};

struct Z_ElementRequestCompositeElementSpecs {
	int num;
	Z_SimpleElement **elements;
};

struct Z_ElementRequestCompositeElement {
	int which;
	union {
		Z_ElementRequestCompositeElementPrimitives *primitives;
		Z_ElementRequestCompositeElementSpecs *specs;
#define Z_ElementRequestCompositeElement_primitives 1
#define Z_ElementRequestCompositeElement_specs 2
	} u;
	Z_ETagPath *deliveryTag;
	Z_Variant *variantRequest; /* OPT */
};

struct Z_ElementRequest {
	int which;
	union {
		Z_SimpleElement *simpleElement;
		Z_ElementRequestCompositeElement *compositeElement;
#define Z_ERequest_simpleElement 1
#define Z_ERequest_compositeElement 2
	} u;
};

struct Z_SimpleElement {
	Z_ETagPath *path;
	Z_Variant *variantRequest; /* OPT */
};

struct Z_SpecificTag {
	Odr_int *tagType; /* OPT */
	Z_StringOrNumeric *tagValue;
	Z_Occurrences *occurrences; /* OPT */
};

struct Z_ETagUnit {
	int which;
	union {
		Z_SpecificTag *specificTag;
		Z_Occurrences *wildThing;
		Odr_null *wildPath;
#define Z_ETagUnit_specificTag 1
#define Z_ETagUnit_wildThing 2
#define Z_ETagUnit_wildPath 3
	} u;
};

struct Z_ETagPath {
	int num_tags;
	Z_ETagUnit **tags;
};

struct Z_OccurValues {
	Odr_int *start;
	Odr_int *howMany; /* OPT */
};

struct Z_Occurrences {
	int which;
	union {
		Odr_null *all;
		Odr_null *last;
		Z_OccurValues *values;
#define Z_Occurrences_all 1
#define Z_Occurrences_last 2
#define Z_Occurrences_values 3
	} u;
};

#ifdef __cplusplus
}
#endif
#endif
