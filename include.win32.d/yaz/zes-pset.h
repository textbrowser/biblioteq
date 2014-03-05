/** \file zes-pset.h
    \brief ASN.1 Module ESFormat-PersistentResultSet

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef zes_pset_H
#define zes_pset_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_PRPersistentResultSetEsRequest Z_PRPersistentResultSetEsRequest;
YAZ_EXPORT int z_PRPersistentResultSetEsRequest(ODR o, Z_PRPersistentResultSetEsRequest **p, int opt, const char *name);

typedef struct Z_PRPersistentResultSetTaskPackage Z_PRPersistentResultSetTaskPackage;
YAZ_EXPORT int z_PRPersistentResultSetTaskPackage(ODR o, Z_PRPersistentResultSetTaskPackage **p, int opt, const char *name);

typedef struct Z_PRPersistentResultSet Z_PRPersistentResultSet;
YAZ_EXPORT int z_PRPersistentResultSet(ODR o, Z_PRPersistentResultSet **p, int opt, const char *name);

typedef struct Z_PROriginPartNotToKeep Z_PROriginPartNotToKeep;
YAZ_EXPORT int z_PROriginPartNotToKeep(ODR o, Z_PROriginPartNotToKeep **p, int opt, const char *name);

typedef struct Z_PRTargetPart Z_PRTargetPart;
YAZ_EXPORT int z_PRTargetPart(ODR o, Z_PRTargetPart **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_PRPersistentResultSetEsRequest {
	Odr_null *toKeep;
	Z_PROriginPartNotToKeep *notToKeep; /* OPT */
};

struct Z_PRPersistentResultSetTaskPackage {
	Odr_null *originPart;
	Z_PRTargetPart *targetPart; /* OPT */
};

struct Z_PRPersistentResultSet {
	int which;
	union {
		Z_PRPersistentResultSetEsRequest *esRequest;
		Z_PRPersistentResultSetTaskPackage *taskPackage;
#define Z_PRPersistentResultSet_esRequest 1
#define Z_PRPersistentResultSet_taskPackage 2
	} u;
};

struct Z_PROriginPartNotToKeep {
	Z_InternationalString *originSuppliedResultSet; /* OPT */
#define Z_PROriginPartNotToKeep_replace 1
#define Z_PROriginPartNotToKeep_append 2
	Odr_int *replaceOrAppend; /* OPT */
};

struct Z_PRTargetPart {
	Z_InternationalString *targetSuppliedResultSet; /* OPT */
	Odr_int *numberOfRecords; /* OPT */
};

#ifdef __cplusplus
}
#endif
#endif
