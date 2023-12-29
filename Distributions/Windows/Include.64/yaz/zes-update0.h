/** \file zes-update0.h
    \brief ASN.1 Module ESFormat-Update0

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef zes_update0_H
#define zes_update0_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_IU0UpdateEsRequest Z_IU0UpdateEsRequest;
YAZ_EXPORT int z_IU0UpdateEsRequest(ODR o, Z_IU0UpdateEsRequest **p, int opt, const char *name);

typedef struct Z_IU0UpdateTaskPackage Z_IU0UpdateTaskPackage;
YAZ_EXPORT int z_IU0UpdateTaskPackage(ODR o, Z_IU0UpdateTaskPackage **p, int opt, const char *name);

typedef struct Z_IU0Update Z_IU0Update;
YAZ_EXPORT int z_IU0Update(ODR o, Z_IU0Update **p, int opt, const char *name);

typedef struct Z_IU0OriginPartToKeep Z_IU0OriginPartToKeep;
YAZ_EXPORT int z_IU0OriginPartToKeep(ODR o, Z_IU0OriginPartToKeep **p, int opt, const char *name);

typedef struct Z_IU0TargetPart Z_IU0TargetPart;
YAZ_EXPORT int z_IU0TargetPart(ODR o, Z_IU0TargetPart **p, int opt, const char *name);

typedef struct Z_IU0SuppliedRecordsId Z_IU0SuppliedRecordsId;
YAZ_EXPORT int z_IU0SuppliedRecordsId(ODR o, Z_IU0SuppliedRecordsId **p, int opt, const char *name);

typedef struct Z_IU0SuppliedRecords_elem Z_IU0SuppliedRecords_elem;
YAZ_EXPORT int z_IU0SuppliedRecords_elem(ODR o, Z_IU0SuppliedRecords_elem **p, int opt, const char *name);

typedef struct Z_IU0SuppliedRecords Z_IU0SuppliedRecords;
YAZ_EXPORT int z_IU0SuppliedRecords(ODR o, Z_IU0SuppliedRecords **p, int opt, const char *name);

typedef Z_IU0SuppliedRecords  Z_IU0OriginPartNotToKeep;
YAZ_EXPORT int z_IU0OriginPartNotToKeep(ODR o, Z_IU0OriginPartNotToKeep **p, int opt, const char *name);

typedef struct Z_IU0CorrelationInfo Z_IU0CorrelationInfo;
YAZ_EXPORT int z_IU0CorrelationInfo(ODR o, Z_IU0CorrelationInfo **p, int opt, const char *name);

typedef struct Z_IU0TaskPackageRecordStructure Z_IU0TaskPackageRecordStructure;
YAZ_EXPORT int z_IU0TaskPackageRecordStructure(ODR o, Z_IU0TaskPackageRecordStructure **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_IU0UpdateEsRequest {
	Z_IU0OriginPartToKeep *toKeep;
	Z_IU0OriginPartNotToKeep *notToKeep;
};

struct Z_IU0UpdateTaskPackage {
	Z_IU0OriginPartToKeep *originPart;
	Z_IU0TargetPart *targetPart;
};

struct Z_IU0Update {
	int which;
	union {
		Z_IU0UpdateEsRequest *esRequest;
		Z_IU0UpdateTaskPackage *taskPackage;
#define Z_IU0Update_esRequest 1
#define Z_IU0Update_taskPackage 2
	} u;
};

struct Z_IU0OriginPartToKeep {
#define Z_IU0OriginPartToKeep_recordInsert 1
#define Z_IU0OriginPartToKeep_recordReplace 2
#define Z_IU0OriginPartToKeep_recordDelete 3
#define Z_IU0OriginPartToKeep_elementUpdate 4
	Odr_int *action;
	Z_InternationalString *databaseName;
	Odr_oid *schema; /* OPT */
	Z_InternationalString *elementSetName; /* OPT */
};



struct Z_IU0TargetPart {
#define Z_IU0TargetPart_success 1
#define Z_IU0TargetPart_partial 2
#define Z_IU0TargetPart_failure 3
	Odr_int *updateStatus;
	int num_globalDiagnostics;
	Z_DiagRec **globalDiagnostics; /* OPT */
	int num_taskPackageRecords;
	Z_IU0TaskPackageRecordStructure **taskPackageRecords; /* OPT */
};

struct Z_IU0SuppliedRecordsId {
	int which;
	union {
		char *timeStamp;
		Z_InternationalString *versionNumber;
		Z_External *previousVersion;
#define Z_IU0SuppliedRecordsId_timeStamp 1
#define Z_IU0SuppliedRecordsId_versionNumber 2
#define Z_IU0SuppliedRecordsId_previousVersion 3
	} u;
};

struct Z_IU0SuppliedRecords_elem {
	int which;
	union {
		Odr_int *number;
		Z_InternationalString *string;
		Odr_oct *opaque;
#define Z_IU0SuppliedRecords_elem_number 1
#define Z_IU0SuppliedRecords_elem_string 2
#define Z_IU0SuppliedRecords_elem_opaque 3
	} u; /* OPT */
	Z_IU0SuppliedRecordsId *supplementalId; /* OPT */
	Z_IU0CorrelationInfo *correlationInfo; /* OPT */
	Z_External *record;
};

struct Z_IU0SuppliedRecords {
	int num;
	Z_IU0SuppliedRecords_elem **elements;
};

struct Z_IU0CorrelationInfo {
	Z_InternationalString *note; /* OPT */
	Odr_int *id; /* OPT */
};

struct Z_IU0TaskPackageRecordStructure {
	int which;
	union {
		Z_External *record;
		Z_DiagRec *diagnostic;
#define Z_IU0TaskPackageRecordStructure_record 1
#define Z_IU0TaskPackageRecordStructure_diagnostic 2
	} u; /* OPT */
	Z_IU0CorrelationInfo *correlationInfo; /* OPT */
#define Z_IU0TaskPackageRecordStructure_success 1
#define Z_IU0TaskPackageRecordStructure_queued 2
#define Z_IU0TaskPackageRecordStructure_inProcess 3
#define Z_IU0TaskPackageRecordStructure_failure 4
	Odr_int *recordStatus;
};

#ifdef __cplusplus
}
#endif
#endif
