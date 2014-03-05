/** \file zes-update.h
    \brief ASN.1 Module ESFormat-Update

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef zes_update_H
#define zes_update_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_IUUpdateEsRequest Z_IUUpdateEsRequest;
YAZ_EXPORT int z_IUUpdateEsRequest(ODR o, Z_IUUpdateEsRequest **p, int opt, const char *name);

typedef struct Z_IUUpdateTaskPackage Z_IUUpdateTaskPackage;
YAZ_EXPORT int z_IUUpdateTaskPackage(ODR o, Z_IUUpdateTaskPackage **p, int opt, const char *name);

typedef struct Z_IUUpdate Z_IUUpdate;
YAZ_EXPORT int z_IUUpdate(ODR o, Z_IUUpdate **p, int opt, const char *name);

typedef struct Z_IUOriginPartToKeep Z_IUOriginPartToKeep;
YAZ_EXPORT int z_IUOriginPartToKeep(ODR o, Z_IUOriginPartToKeep **p, int opt, const char *name);

typedef struct Z_IUTargetPart Z_IUTargetPart;
YAZ_EXPORT int z_IUTargetPart(ODR o, Z_IUTargetPart **p, int opt, const char *name);

typedef struct Z_IUSuppliedRecordsId Z_IUSuppliedRecordsId;
YAZ_EXPORT int z_IUSuppliedRecordsId(ODR o, Z_IUSuppliedRecordsId **p, int opt, const char *name);

typedef struct Z_IUSuppliedRecords_elem Z_IUSuppliedRecords_elem;
YAZ_EXPORT int z_IUSuppliedRecords_elem(ODR o, Z_IUSuppliedRecords_elem **p, int opt, const char *name);

typedef struct Z_IUSuppliedRecords Z_IUSuppliedRecords;
YAZ_EXPORT int z_IUSuppliedRecords(ODR o, Z_IUSuppliedRecords **p, int opt, const char *name);

typedef Z_IUSuppliedRecords  Z_IUOriginPartNotToKeep;
YAZ_EXPORT int z_IUOriginPartNotToKeep(ODR o, Z_IUOriginPartNotToKeep **p, int opt, const char *name);

typedef struct Z_IUCorrelationInfo Z_IUCorrelationInfo;
YAZ_EXPORT int z_IUCorrelationInfo(ODR o, Z_IUCorrelationInfo **p, int opt, const char *name);

typedef struct Z_IUTaskPackageRecordStructureSurrogateDiagnostics Z_IUTaskPackageRecordStructureSurrogateDiagnostics;
YAZ_EXPORT int z_IUTaskPackageRecordStructureSurrogateDiagnostics(ODR o, Z_IUTaskPackageRecordStructureSurrogateDiagnostics **p, int opt, const char *name);

typedef struct Z_IUTaskPackageRecordStructure Z_IUTaskPackageRecordStructure;
YAZ_EXPORT int z_IUTaskPackageRecordStructure(ODR o, Z_IUTaskPackageRecordStructure **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_IUUpdateEsRequest {
	Z_IUOriginPartToKeep *toKeep;
	Z_IUOriginPartNotToKeep *notToKeep;
};

struct Z_IUUpdateTaskPackage {
	Z_IUOriginPartToKeep *originPart;
	Z_IUTargetPart *targetPart;
};

struct Z_IUUpdate {
	int which;
	union {
		Z_IUUpdateEsRequest *esRequest;
		Z_IUUpdateTaskPackage *taskPackage;
#define Z_IUUpdate_esRequest 1
#define Z_IUUpdate_taskPackage 2
	} u;
};

struct Z_IUOriginPartToKeep {
#define Z_IUOriginPartToKeep_recordInsert 1
#define Z_IUOriginPartToKeep_recordReplace 2
#define Z_IUOriginPartToKeep_recordDelete 3
#define Z_IUOriginPartToKeep_elementUpdate 4
#define Z_IUOriginPartToKeep_specialUpdate 5
	Odr_int *action;
	Z_InternationalString *databaseName;
	Odr_oid *schema; /* OPT */
	Z_InternationalString *elementSetName; /* OPT */
	Z_External *actionQualifier; /* OPT */
};



struct Z_IUTargetPart {
#define Z_IUTargetPart_success 1
#define Z_IUTargetPart_partial 2
#define Z_IUTargetPart_failure 3
	Odr_int *updateStatus;
	int num_globalDiagnostics;
	Z_DiagRec **globalDiagnostics; /* OPT */
	int num_taskPackageRecords;
	Z_IUTaskPackageRecordStructure **taskPackageRecords;
};

struct Z_IUSuppliedRecordsId {
	int which;
	union {
		char *timeStamp;
		Z_InternationalString *versionNumber;
		Z_External *previousVersion;
#define Z_IUSuppliedRecordsId_timeStamp 1
#define Z_IUSuppliedRecordsId_versionNumber 2
#define Z_IUSuppliedRecordsId_previousVersion 3
	} u;
};

struct Z_IUSuppliedRecords_elem {
	int which;
	union {
		Odr_int *number;
		Z_InternationalString *string;
		Odr_oct *opaque;
#define Z_IUSuppliedRecords_elem_number 1
#define Z_IUSuppliedRecords_elem_string 2
#define Z_IUSuppliedRecords_elem_opaque 3
	} u; /* OPT */
	Z_IUSuppliedRecordsId *supplementalId; /* OPT */
	Z_IUCorrelationInfo *correlationInfo; /* OPT */
	Z_External *record;
};

struct Z_IUSuppliedRecords {
	int num;
	Z_IUSuppliedRecords_elem **elements;
};

struct Z_IUCorrelationInfo {
	Z_InternationalString *note; /* OPT */
	Odr_int *id; /* OPT */
};

struct Z_IUTaskPackageRecordStructureSurrogateDiagnostics {
	int num;
	Z_DiagRec **elements;
};

struct Z_IUTaskPackageRecordStructure {
	int which;
	union {
		Z_External *record;
		Z_IUTaskPackageRecordStructureSurrogateDiagnostics *surrogateDiagnostics;
#define Z_IUTaskPackageRecordStructure_record 1
#define Z_IUTaskPackageRecordStructure_surrogateDiagnostics 2
	} u; /* OPT */
	Z_IUCorrelationInfo *correlationInfo; /* OPT */
#define Z_IUTaskPackageRecordStructure_success 1
#define Z_IUTaskPackageRecordStructure_queued 2
#define Z_IUTaskPackageRecordStructure_inProcess 3
#define Z_IUTaskPackageRecordStructure_failure 4
	Odr_int *recordStatus;
	int num_supplementalDiagnostics;
	Z_DiagRec **supplementalDiagnostics; /* OPT */
};

#ifdef __cplusplus
}
#endif
#endif
