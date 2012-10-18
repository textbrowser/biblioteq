/** \file zes-pquery.h
    \brief ASN.1 Module ESFormat-PersistentQuery

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef zes_pquery_H
#define zes_pquery_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_PQueryPersistentQueryEsRequest Z_PQueryPersistentQueryEsRequest;
YAZ_EXPORT int z_PQueryPersistentQueryEsRequest(ODR o, Z_PQueryPersistentQueryEsRequest **p, int opt, const char *name);

typedef struct Z_PQueryPersistentQueryTaskPackage Z_PQueryPersistentQueryTaskPackage;
YAZ_EXPORT int z_PQueryPersistentQueryTaskPackage(ODR o, Z_PQueryPersistentQueryTaskPackage **p, int opt, const char *name);

typedef struct Z_PQueryPersistentQuery Z_PQueryPersistentQuery;
YAZ_EXPORT int z_PQueryPersistentQuery(ODR o, Z_PQueryPersistentQuery **p, int opt, const char *name);

typedef struct Z_PQueryOriginPartToKeep Z_PQueryOriginPartToKeep;
YAZ_EXPORT int z_PQueryOriginPartToKeep(ODR o, Z_PQueryOriginPartToKeep **p, int opt, const char *name);

typedef struct Z_PQueryOriginPartNotToKeep Z_PQueryOriginPartNotToKeep;
YAZ_EXPORT int z_PQueryOriginPartNotToKeep(ODR o, Z_PQueryOriginPartNotToKeep **p, int opt, const char *name);

typedef Z_Query  Z_PQueryTargetPart;
YAZ_EXPORT int z_PQueryTargetPart(ODR o, Z_PQueryTargetPart **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_PQueryPersistentQueryEsRequest {
	Z_PQueryOriginPartToKeep *toKeep; /* OPT */
	Z_PQueryOriginPartNotToKeep *notToKeep;
};

struct Z_PQueryPersistentQueryTaskPackage {
	Z_PQueryOriginPartToKeep *originPart; /* OPT */
	Z_PQueryTargetPart *targetPart;
};

struct Z_PQueryPersistentQuery {
	int which;
	union {
		Z_PQueryPersistentQueryEsRequest *esRequest;
		Z_PQueryPersistentQueryTaskPackage *taskPackage;
#define Z_PQueryPersistentQuery_esRequest 1
#define Z_PQueryPersistentQuery_taskPackage 2
	} u;
};

struct Z_PQueryOriginPartToKeep {
	int num_dbNames;
	Z_InternationalString **dbNames; /* OPT */
	Z_OtherInformation *additionalSearchInfo; /* OPT */
};

struct Z_PQueryOriginPartNotToKeep {
	int which;
	union {
		Z_InternationalString *package;
		Z_Query *query;
#define Z_PQueryOriginPartNotToKeep_package 1
#define Z_PQueryOriginPartNotToKeep_query 2
	} u;
};



#ifdef __cplusplus
}
#endif
#endif
