/** \file z-diag1.h
    \brief ASN.1 Module DiagnosticFormatDiag1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_diag1_H
#define z_diag1_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_DiagnosticFormat_s Z_DiagnosticFormat_s;
YAZ_EXPORT int z_DiagnosticFormat_s(ODR o, Z_DiagnosticFormat_s **p, int opt, const char *name);

typedef struct Z_DiagnosticFormat Z_DiagnosticFormat;
YAZ_EXPORT int z_DiagnosticFormat(ODR o, Z_DiagnosticFormat **p, int opt, const char *name);

typedef struct Z_TooMany Z_TooMany;
YAZ_EXPORT int z_TooMany(ODR o, Z_TooMany **p, int opt, const char *name);

typedef struct Z_BadSpec Z_BadSpec;
YAZ_EXPORT int z_BadSpec(ODR o, Z_BadSpec **p, int opt, const char *name);

typedef struct Z_DbUnavail_0 Z_DbUnavail_0;
YAZ_EXPORT int z_DbUnavail_0(ODR o, Z_DbUnavail_0 **p, int opt, const char *name);

typedef struct Z_DbUnavail Z_DbUnavail;
YAZ_EXPORT int z_DbUnavail(ODR o, Z_DbUnavail **p, int opt, const char *name);

typedef struct Z_Attribute Z_Attribute;
YAZ_EXPORT int z_Attribute(ODR o, Z_Attribute **p, int opt, const char *name);

typedef struct Z_AttCombo Z_AttCombo;
YAZ_EXPORT int z_AttCombo(ODR o, Z_AttCombo **p, int opt, const char *name);

typedef struct Z_DiagTerm Z_DiagTerm;
YAZ_EXPORT int z_DiagTerm(ODR o, Z_DiagTerm **p, int opt, const char *name);

typedef struct Z_Proximity Z_Proximity;
YAZ_EXPORT int z_Proximity(ODR o, Z_Proximity **p, int opt, const char *name);

typedef struct Z_AttrListList Z_AttrListList;
YAZ_EXPORT int z_AttrListList(ODR o, Z_AttrListList **p, int opt, const char *name);

typedef struct Z_Scan Z_Scan;
YAZ_EXPORT int z_Scan(ODR o, Z_Scan **p, int opt, const char *name);

typedef struct Z_StringList Z_StringList;
YAZ_EXPORT int z_StringList(ODR o, Z_StringList **p, int opt, const char *name);

typedef struct Z_Sort Z_Sort;
YAZ_EXPORT int z_Sort(ODR o, Z_Sort **p, int opt, const char *name);

typedef struct Z_Segmentation Z_Segmentation;
YAZ_EXPORT int z_Segmentation(ODR o, Z_Segmentation **p, int opt, const char *name);

typedef struct Z_ExtServices Z_ExtServices;
YAZ_EXPORT int z_ExtServices(ODR o, Z_ExtServices **p, int opt, const char *name);

typedef struct Z_OidList Z_OidList;
YAZ_EXPORT int z_OidList(ODR o, Z_OidList **p, int opt, const char *name);

typedef struct Z_AltOidList Z_AltOidList;
YAZ_EXPORT int z_AltOidList(ODR o, Z_AltOidList **p, int opt, const char *name);

typedef struct Z_AccessCtrl Z_AccessCtrl;
YAZ_EXPORT int z_AccessCtrl(ODR o, Z_AccessCtrl **p, int opt, const char *name);

typedef struct Z_RecordSyntax Z_RecordSyntax;
YAZ_EXPORT int z_RecordSyntax(ODR o, Z_RecordSyntax **p, int opt, const char *name);

typedef struct Z_DiagFormat Z_DiagFormat;
YAZ_EXPORT int z_DiagFormat(ODR o, Z_DiagFormat **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_DiagnosticFormat_s {
	int which;
	union {
		Z_DefaultDiagFormat *defaultDiagRec;
		Z_DiagFormat *explicitDiagnostic;
#define Z_DiagnosticFormat_s_defaultDiagRec 1
#define Z_DiagnosticFormat_s_explicitDiagnostic 2
	} u; /* OPT */
	Z_InternationalString *message; /* OPT */
};

struct Z_DiagnosticFormat {
	int num;
	Z_DiagnosticFormat_s **elements;
};

struct Z_TooMany {
#define Z_TooMany_argumentWords 1
#define Z_TooMany_truncatedWords 2
#define Z_TooMany_booleanOperators 3
#define Z_TooMany_incompleteSubfields 4
#define Z_TooMany_characters 5
#define Z_TooMany_recordsRetrieved 6
#define Z_TooMany_dataBasesSpecified 7
#define Z_TooMany_resultSetsCreated 8
#define Z_TooMany_indexTermsProcessed 9
	Odr_int *tooManyWhat;
	Odr_int *max; /* OPT */
};

struct Z_BadSpec {
	Z_Specification *spec;
	Z_DatabaseName *db; /* OPT */
	int num_goodOnes;
	Z_Specification **goodOnes; /* OPT */
};

struct Z_DbUnavail_0 {
#define Z_DbUnavail_0_doesNotExist 0
#define Z_DbUnavail_0_existsButUnavail 1
#define Z_DbUnavail_0_locked 2
#define Z_DbUnavail_0_accessDenied 3
	Odr_int *reasonCode; /* OPT */
	Z_InternationalString *message; /* OPT */
};

struct Z_DbUnavail {
	Z_DatabaseName *db;
	Z_DbUnavail_0 *why;
};

struct Z_Attribute {
	Odr_oid *id;
	Odr_int *type; /* OPT */
	Odr_int *value; /* OPT */
	Z_Term *term; /* OPT */
};

struct Z_AttCombo {
	Z_AttributeList *unsupportedCombination;
	int num_recommendedAlternatives;
	Z_AttributeList **recommendedAlternatives; /* OPT */
};

struct Z_DiagTerm {
#define Z_DiagTerm_codedValue 1
#define Z_DiagTerm_unparsable 2
#define Z_DiagTerm_tooShort 3
#define Z_DiagTerm_type 4
	Odr_int *problem; /* OPT */
	Z_Term *term;
};

struct Z_Proximity {
	int which;
	union {
		Odr_null *resultSets;
		Z_InternationalString *badSet;
		Odr_int *relation;
		Odr_int *unit;
		Odr_int *distance;
		Z_AttributeList *attributes;
		Odr_null *ordered;
		Odr_null *exclusion;
#define Z_Proximity_resultSets 1
#define Z_Proximity_badSet 2
#define Z_Proximity_relation 3
#define Z_Proximity_unit 4
#define Z_Proximity_distance 5
#define Z_Proximity_attributes 6
#define Z_Proximity_ordered 7
#define Z_Proximity_exclusion 8
	} u;
};

struct Z_AttrListList {
	int num;
	Z_AttributeList **elements;
};

struct Z_Scan {
	int which;
	union {
		Odr_null *nonZeroStepSize;
		Odr_null *specifiedStepSize;
		Odr_null *termList1;
		Z_AttrListList *termList2;
#define Z_Scan_mustBeOne 1
#define Z_Scan_mustBePositive 2
#define Z_Scan_mustBeNonNegative 3
#define Z_Scan_other 4
		Odr_int *posInResponse;
		Odr_null *resources;
		Odr_null *endOfList;
#define Z_Scan_nonZeroStepSize 1
#define Z_Scan_specifiedStepSize 2
#define Z_Scan_termList1 3
#define Z_Scan_termList2 4
#define Z_Scan_posInResponse 5
#define Z_Scan_resources 6
#define Z_Scan_endOfList 7
	} u;
};

struct Z_StringList {
	int num;
	Z_InternationalString **elements;
};

struct Z_Sort {
	int which;
	union {
		Odr_null *sequence;
		Odr_null *noRsName;
		Odr_int *tooMany;
		Odr_null *incompatible;
		Odr_null *generic;
		Odr_null *dbSpecific;
		Z_SortElement *sortElement;
#define Z_Sort_tooMany 1
#define Z_Sort_duplicate 2
		Odr_int *key;
		Odr_null *action;
#define Z_Sort_relation 1
#define Z_Sort_case 2
#define Z_Sort_action 3
#define Z_Sort_sort 4
		Odr_int *illegal;
		Z_StringList *inputTooLarge;
		Odr_null *aggregateTooLarge;
#define Z_SortD_sequence 1
#define Z_SortD_noRsName 2
#define Z_SortD_tooMany 3
#define Z_SortD_incompatible 4
#define Z_SortD_generic 5
#define Z_SortD_dbSpecific 6
#define Z_SortD_sortElement 7
#define Z_SortD_key 8
#define Z_SortD_action 9
#define Z_SortD_illegal 10
#define Z_SortD_inputTooLarge 11
#define Z_SortD_aggregateTooLarge 12
	} u;
};

struct Z_Segmentation {
	int which;
	union {
		Odr_null *segmentCount;
		Odr_int *segmentSize;
#define Z_Segmentation_segmentCount 1
#define Z_Segmentation_segmentSize 2
	} u;
};

struct Z_ExtServices {
	int which;
	union {
#define Z_ExtServices_nameInUse 1
#define Z_ExtServices_noSuchName 2
#define Z_ExtServices_quota 3
#define Z_ExtServices_type 4
		Odr_int *req;
#define Z_ExtServices_id 1
#define Z_ExtServices_modifyDelete 2
		Odr_int *permission;
#define Z_ExtServices_failed 1
#define Z_ExtServices_service 2
#define Z_ExtServices_parameters 3
		Odr_int *immediate;
#define Z_ExtServices_req 1
#define Z_ExtServices_permission 2
#define Z_ExtServices_immediate 3
	} u;
};

struct Z_OidList {
	int num;
	Odr_oid **elements;
};

struct Z_AltOidList {
	int num;
	Odr_oid **elements;
};

struct Z_AccessCtrl {
	int which;
	union {
		Odr_null *noUser;
		Odr_null *refused;
		Odr_null *simple;
		Z_OidList *oid;
		Z_AltOidList *alternative;
		Odr_null *pwdInv;
		Odr_null *pwdExp;
#define Z_AccessCtrl_noUser 1
#define Z_AccessCtrl_refused 2
#define Z_AccessCtrl_simple 3
#define Z_AccessCtrl_oid 4
#define Z_AccessCtrl_alternative 5
#define Z_AccessCtrl_pwdInv 6
#define Z_AccessCtrl_pwdExp 7
	} u;
};

struct Z_RecordSyntax {
	Odr_oid *unsupportedSyntax;
	int num_suggestedAlternatives;
	Odr_oid **suggestedAlternatives; /* OPT */
};

struct Z_DiagFormat {
	int which;
	union {
		Z_TooMany *tooMany;
		Z_BadSpec *badSpec;
		Z_DbUnavail *dbUnavail;
#define Z_DiagFormat_and 0
#define Z_DiagFormat_or 1
#define Z_DiagFormat_and_not 2
#define Z_DiagFormat_prox 3
		Odr_int *unSupOp;
		Z_Attribute *attribute;
		Z_AttCombo *attCombo;
		Z_DiagTerm *term;
		Z_Proximity *proximity;
		Z_Scan *scan;
		Z_Sort *sort;
		Z_Segmentation *segmentation;
		Z_ExtServices *extServices;
		Z_AccessCtrl *accessCtrl;
		Z_RecordSyntax *recordSyntax;
#define Z_DiagFormat_tooMany 1
#define Z_DiagFormat_badSpec 2
#define Z_DiagFormat_dbUnavail 3
#define Z_DiagFormat_unSupOp 4
#define Z_DiagFormat_attribute 5
#define Z_DiagFormat_attCombo 6
#define Z_DiagFormat_term 7
#define Z_DiagFormat_proximity 8
#define Z_DiagFormat_scan 9
#define Z_DiagFormat_sort 10
#define Z_DiagFormat_segmentation 11
#define Z_DiagFormat_extServices 12
#define Z_DiagFormat_accessCtrl 13
#define Z_DiagFormat_recordSyntax 14
	} u;
};

#ifdef __cplusplus
}
#endif
#endif
