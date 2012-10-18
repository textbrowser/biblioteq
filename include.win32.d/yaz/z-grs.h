/** \file z-grs.h
    \brief ASN.1 Module RecordSyntax-generic

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_grs_H
#define z_grs_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_GenericRecord Z_GenericRecord;
YAZ_EXPORT int z_GenericRecord(ODR o, Z_GenericRecord **p, int opt, const char *name);

typedef struct Z_TaggedElement Z_TaggedElement;
YAZ_EXPORT int z_TaggedElement(ODR o, Z_TaggedElement **p, int opt, const char *name);

typedef struct Z_ElementData Z_ElementData;
YAZ_EXPORT int z_ElementData(ODR o, Z_ElementData **p, int opt, const char *name);

typedef struct Z_ElementMetaData Z_ElementMetaData;
YAZ_EXPORT int z_ElementMetaData(ODR o, Z_ElementMetaData **p, int opt, const char *name);

typedef struct Z_TagPath_s Z_TagPath_s;
YAZ_EXPORT int z_TagPath_s(ODR o, Z_TagPath_s **p, int opt, const char *name);

typedef struct Z_TagPath Z_TagPath;
YAZ_EXPORT int z_TagPath(ODR o, Z_TagPath **p, int opt, const char *name);

typedef struct Z_Order Z_Order;
YAZ_EXPORT int z_Order(ODR o, Z_Order **p, int opt, const char *name);

typedef struct Z_Usage Z_Usage;
YAZ_EXPORT int z_Usage(ODR o, Z_Usage **p, int opt, const char *name);

typedef struct Z_HitVector Z_HitVector;
YAZ_EXPORT int z_HitVector(ODR o, Z_HitVector **p, int opt, const char *name);

typedef struct Z_Triple Z_Triple;
YAZ_EXPORT int z_Triple(ODR o, Z_Triple **p, int opt, const char *name);

typedef struct Z_Variant Z_Variant;
YAZ_EXPORT int z_Variant(ODR o, Z_Variant **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_GenericRecord {
	int num_elements;
	Z_TaggedElement **elements;
};

struct Z_TaggedElement {
	Odr_int *tagType; /* OPT */
	Z_StringOrNumeric *tagValue;
	Odr_int *tagOccurrence; /* OPT */
	Z_ElementData *content;
	Z_ElementMetaData *metaData; /* OPT */
	Z_Variant *appliedVariant; /* OPT */
};

struct Z_ElementData {
	int which;
	union {
		Odr_oct *octets;
		Odr_int *numeric;
		char *date;
		Z_External *ext;
		Z_InternationalString *string;
		Odr_bool *trueOrFalse;
		Odr_oid *oid;
		Z_IntUnit *intUnit;
		Odr_null *elementNotThere;
		Odr_null *elementEmpty;
		Odr_null *noDataRequested;
		Z_External *diagnostic;
		Z_GenericRecord *subtree;
#define Z_ElementData_octets 1
#define Z_ElementData_numeric 2
#define Z_ElementData_date 3
#define Z_ElementData_ext 4
#define Z_ElementData_string 5
#define Z_ElementData_trueOrFalse 6
#define Z_ElementData_oid 7
#define Z_ElementData_intUnit 8
#define Z_ElementData_elementNotThere 9
#define Z_ElementData_elementEmpty 10
#define Z_ElementData_noDataRequested 11
#define Z_ElementData_diagnostic 12
#define Z_ElementData_subtree 13
	} u;
};

struct Z_ElementMetaData {
	Z_Order *seriesOrder; /* OPT */
	Z_Usage *usageRight; /* OPT */
	int num_hits;
	Z_HitVector **hits; /* OPT */
	Z_InternationalString *displayName; /* OPT */
	int num_supportedVariants;
	Z_Variant **supportedVariants; /* OPT */
	Z_InternationalString *message; /* OPT */
	Odr_oct *elementDescriptor; /* OPT */
	Z_TagPath *surrogateFor; /* OPT */
	Z_TagPath *surrogateElement; /* OPT */
	Z_External *other; /* OPT */
};

struct Z_TagPath_s {
	Odr_int *tagType; /* OPT */
	Z_StringOrNumeric *tagValue;
	Odr_int *tagOccurrence; /* OPT */
};

struct Z_TagPath {
	int num;
	Z_TagPath_s **elements;
};

struct Z_Order {
	Odr_bool *ascending;
	Odr_int *order;
};

struct Z_Usage {
#define Z_Usage_redistributable 1
#define Z_Usage_restricted 2
#define Z_Usage_licensePointer 3
	Odr_int *type;
	Z_InternationalString *restriction; /* OPT */
};

struct Z_HitVector {
	Z_Term *satisfier; /* OPT */
	Z_IntUnit *offsetIntoElement; /* OPT */
	Z_IntUnit *length; /* OPT */
	Odr_int *hitRank; /* OPT */
	Odr_oct *targetToken; /* OPT */
};

struct Z_Triple {
	Odr_oid *variantSetId; /* OPT */
	Odr_int *zclass;
	Odr_int *type;
	int which;
	union {
		Odr_int *integer;
		Z_InternationalString *internationalString;
		Odr_oct *octetString;
		Odr_oid *objectIdentifier;
		Odr_bool *boolean;
		Odr_null *null;
		Z_Unit *unit;
		Z_IntUnit *valueAndUnit;
#define Z_Triple_integer 1
#define Z_Triple_internationalString 2
#define Z_Triple_octetString 3
#define Z_Triple_objectIdentifier 4
#define Z_Triple_boolean 5
#define Z_Triple_null 6
#define Z_Triple_unit 7
#define Z_Triple_valueAndUnit 8
	} value;
};

struct Z_Variant {
	Odr_oid *globalVariantSetId; /* OPT */
	int num_triples;
	Z_Triple **triples;
};

#ifdef __cplusplus
}
#endif
#endif
