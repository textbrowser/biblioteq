/** \file zes-expi.h
    \brief ASN.1 Module ESFormat-ExportInvocation

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef zes_expi_H
#define zes_expi_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#include <yaz/zes-exps.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_EIExportInvocationEsRequest Z_EIExportInvocationEsRequest;
YAZ_EXPORT int z_EIExportInvocationEsRequest(ODR o, Z_EIExportInvocationEsRequest **p, int opt, const char *name);

typedef struct Z_EIExportInvocationTaskPackage Z_EIExportInvocationTaskPackage;
YAZ_EXPORT int z_EIExportInvocationTaskPackage(ODR o, Z_EIExportInvocationTaskPackage **p, int opt, const char *name);

typedef struct Z_EIExportInvocation Z_EIExportInvocation;
YAZ_EXPORT int z_EIExportInvocation(ODR o, Z_EIExportInvocation **p, int opt, const char *name);

typedef struct Z_EIOriginPartToKeep Z_EIOriginPartToKeep;
YAZ_EXPORT int z_EIOriginPartToKeep(ODR o, Z_EIOriginPartToKeep **p, int opt, const char *name);

typedef struct Z_EIOriginPartNotToKeepRanges_s Z_EIOriginPartNotToKeepRanges_s;
YAZ_EXPORT int z_EIOriginPartNotToKeepRanges_s(ODR o, Z_EIOriginPartNotToKeepRanges_s **p, int opt, const char *name);

typedef struct Z_EIOriginPartNotToKeepRanges Z_EIOriginPartNotToKeepRanges;
YAZ_EXPORT int z_EIOriginPartNotToKeepRanges(ODR o, Z_EIOriginPartNotToKeepRanges **p, int opt, const char *name);

typedef struct Z_EIOriginPartNotToKeep Z_EIOriginPartNotToKeep;
YAZ_EXPORT int z_EIOriginPartNotToKeep(ODR o, Z_EIOriginPartNotToKeep **p, int opt, const char *name);

typedef struct Z_EITargetPart Z_EITargetPart;
YAZ_EXPORT int z_EITargetPart(ODR o, Z_EITargetPart **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_EIExportInvocationEsRequest {
	Z_EIOriginPartToKeep *toKeep;
	Z_EIOriginPartNotToKeep *notToKeep;
};

struct Z_EIExportInvocationTaskPackage {
	Z_EIOriginPartToKeep *originPart;
	Z_EITargetPart *targetPart; /* OPT */
};

struct Z_EIExportInvocation {
	int which;
	union {
		Z_EIExportInvocationEsRequest *esRequest;
		Z_EIExportInvocationTaskPackage *taskPackage;
#define Z_EIExportInvocation_esRequest 1
#define Z_EIExportInvocation_taskPackage 2
	} u;
};

struct Z_EIOriginPartToKeep {
	int which;
	union {
		Z_InternationalString *packageName;
		Z_ESExportSpecification *packageSpec;
#define Z_EIOriginPartToKeep_packageName 1
#define Z_EIOriginPartToKeep_packageSpec 2
	} u;
	Odr_int *numberOfCopies;
};

struct Z_EIOriginPartNotToKeepRanges_s {
	Odr_int *start;
	Odr_int *count; /* OPT */
};

struct Z_EIOriginPartNotToKeepRanges {
	int num;
	Z_EIOriginPartNotToKeepRanges_s **elements;
};

struct Z_EIOriginPartNotToKeep {
	Z_InternationalString *resultSetId;
	int which;
	union {
		Odr_null *all;
		Z_EIOriginPartNotToKeepRanges *ranges;
#define Z_EIOriginPartNotToKeep_all 1
#define Z_EIOriginPartNotToKeep_ranges 2
	} u;
};

struct Z_EITargetPart {
	Z_IntUnit *estimatedQuantity; /* OPT */
	Z_IntUnit *quantitySoFar; /* OPT */
	Z_IntUnit *estimatedCost; /* OPT */
	Z_IntUnit *costSoFar; /* OPT */
};

#ifdef __cplusplus
}
#endif
#endif
