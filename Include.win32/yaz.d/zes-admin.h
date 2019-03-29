/** \file zes-admin.h
    \brief ASN.1 Module ESFormat-Admin

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef zes_admin_H
#define zes_admin_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_AdminEsRequest Z_AdminEsRequest;
YAZ_EXPORT int z_AdminEsRequest(ODR o, Z_AdminEsRequest **p, int opt, const char *name);

typedef struct Z_AdminTaskPackage Z_AdminTaskPackage;
YAZ_EXPORT int z_AdminTaskPackage(ODR o, Z_AdminTaskPackage **p, int opt, const char *name);

typedef struct Z_Admin Z_Admin;
YAZ_EXPORT int z_Admin(ODR o, Z_Admin **p, int opt, const char *name);

typedef struct Z_ESAdminOriginPartToKeep Z_ESAdminOriginPartToKeep;
YAZ_EXPORT int z_ESAdminOriginPartToKeep(ODR o, Z_ESAdminOriginPartToKeep **p, int opt, const char *name);

typedef struct Z_ESAdminOriginPartNotToKeep Z_ESAdminOriginPartNotToKeep;
YAZ_EXPORT int z_ESAdminOriginPartNotToKeep(ODR o, Z_ESAdminOriginPartNotToKeep **p, int opt, const char *name);

typedef struct Z_ESAdminTargetPart Z_ESAdminTargetPart;
YAZ_EXPORT int z_ESAdminTargetPart(ODR o, Z_ESAdminTargetPart **p, int opt, const char *name);

typedef struct Z_ImportParameters Z_ImportParameters;
YAZ_EXPORT int z_ImportParameters(ODR o, Z_ImportParameters **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_AdminEsRequest {
	Z_ESAdminOriginPartToKeep *toKeep;
	Z_ESAdminOriginPartNotToKeep *notToKeep;
};

struct Z_AdminTaskPackage {
	Z_ESAdminOriginPartToKeep *originPart;
	Z_ESAdminTargetPart *targetPart;
};

struct Z_Admin {
	int which;
	union {
		Z_AdminEsRequest *esRequest;
		Z_AdminTaskPackage *taskPackage;
#define Z_Admin_esRequest 1
#define Z_Admin_taskPackage 2
	} u;
};

struct Z_ESAdminOriginPartToKeep {
	int which;
	union {
		Odr_null *reIndex;
		Odr_null *truncate;
		Odr_null *drop;
		Odr_null *create;
		Z_ImportParameters *import;
		Odr_null *refresh;
		Odr_null *commit;
		Odr_null *shutdown;
		Odr_null *start;
#define Z_ESAdminOriginPartToKeep_reIndex 1
#define Z_ESAdminOriginPartToKeep_truncate 2
#define Z_ESAdminOriginPartToKeep_drop 3
#define Z_ESAdminOriginPartToKeep_create 4
#define Z_ESAdminOriginPartToKeep_import 5
#define Z_ESAdminOriginPartToKeep_refresh 6
#define Z_ESAdminOriginPartToKeep_commit 7
#define Z_ESAdminOriginPartToKeep_shutdown 8
#define Z_ESAdminOriginPartToKeep_start 9
	} u;
	Z_InternationalString *databaseName; /* OPT */
};

struct Z_ESAdminOriginPartNotToKeep {
	int which;
	union {
		Z_Segment *records;
		Odr_null *recordsWillFollow;
#define Z_ESAdminOriginPartNotToKeep_records 1
#define Z_ESAdminOriginPartNotToKeep_recordsWillFollow 2
	} u;
};

struct Z_ESAdminTargetPart {
#define Z_ESAdminTargetPart_success 1
#define Z_ESAdminTargetPart_partial 2
#define Z_ESAdminTargetPart_failure 3
	Odr_int *updateStatus;
	int num_globalDiagnostics;
	Z_DiagRec **globalDiagnostics; /* OPT */
};

struct Z_ImportParameters {
	Z_InternationalString *recordType;
};

#ifdef __cplusplus
}
#endif
#endif
