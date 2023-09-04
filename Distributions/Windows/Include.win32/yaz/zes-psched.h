/** \file zes-psched.h
    \brief ASN.1 Module ESFormat-PeriodicQuerySchedule

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef zes_psched_H
#define zes_psched_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#include <yaz/zes-exps.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_PQSPeriodicQueryScheduleEsRequest Z_PQSPeriodicQueryScheduleEsRequest;
YAZ_EXPORT int z_PQSPeriodicQueryScheduleEsRequest(ODR o, Z_PQSPeriodicQueryScheduleEsRequest **p, int opt, const char *name);

typedef struct Z_PQSPeriodicQueryScheduleTaskPackage Z_PQSPeriodicQueryScheduleTaskPackage;
YAZ_EXPORT int z_PQSPeriodicQueryScheduleTaskPackage(ODR o, Z_PQSPeriodicQueryScheduleTaskPackage **p, int opt, const char *name);

typedef struct Z_PQSPeriodicQuerySchedule Z_PQSPeriodicQuerySchedule;
YAZ_EXPORT int z_PQSPeriodicQuerySchedule(ODR o, Z_PQSPeriodicQuerySchedule **p, int opt, const char *name);

typedef struct Z_PQSOriginPartToKeep Z_PQSOriginPartToKeep;
YAZ_EXPORT int z_PQSOriginPartToKeep(ODR o, Z_PQSOriginPartToKeep **p, int opt, const char *name);

typedef struct Z_PQSOriginPartNotToKeep Z_PQSOriginPartNotToKeep;
YAZ_EXPORT int z_PQSOriginPartNotToKeep(ODR o, Z_PQSOriginPartNotToKeep **p, int opt, const char *name);

typedef struct Z_PQSTargetPart Z_PQSTargetPart;
YAZ_EXPORT int z_PQSTargetPart(ODR o, Z_PQSTargetPart **p, int opt, const char *name);

typedef struct Z_PQSPeriod Z_PQSPeriod;
YAZ_EXPORT int z_PQSPeriod(ODR o, Z_PQSPeriod **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_PQSPeriodicQueryScheduleEsRequest {
	Z_PQSOriginPartToKeep *toKeep;
	Z_PQSOriginPartNotToKeep *notToKeep;
};

struct Z_PQSPeriodicQueryScheduleTaskPackage {
	Z_PQSOriginPartToKeep *originPart;
	Z_PQSTargetPart *targetPart;
};

struct Z_PQSPeriodicQuerySchedule {
	int which;
	union {
		Z_PQSPeriodicQueryScheduleEsRequest *esRequest;
		Z_PQSPeriodicQueryScheduleTaskPackage *taskPackage;
#define Z_PQSPeriodicQuerySchedule_esRequest 1
#define Z_PQSPeriodicQuerySchedule_taskPackage 2
	} u;
};

struct Z_PQSOriginPartToKeep {
	Odr_bool *activeFlag;
	int num_databaseNames;
	Z_InternationalString **databaseNames; /* OPT */
#define Z_PQSOriginPartToKeep_replace 1
#define Z_PQSOriginPartToKeep_append 2
#define Z_PQSOriginPartToKeep_createNew 3
	Odr_int *resultSetDisposition; /* OPT */
	Z_ESDestination *alertDestination; /* OPT */
	int which;
	union {
		Z_InternationalString *packageName;
		Z_ESExportSpecification *exportPackage;
#define Z_PQSOriginPartToKeep_packageName 1
#define Z_PQSOriginPartToKeep_exportPackage 2
	} u; /* OPT */
};

struct Z_PQSOriginPartNotToKeep {
	int which;
	union {
		Z_Query *actualQuery;
		Z_InternationalString *packageName;
#define Z_PQSOriginPartNotToKeep_actualQuery 1
#define Z_PQSOriginPartNotToKeep_packageName 2
	} u; /* OPT */
	Z_PQSPeriod *originSuggestedPeriod; /* OPT */
	char *expiration; /* OPT */
	Z_InternationalString *resultSetPackage; /* OPT */
};

struct Z_PQSTargetPart {
	Z_Query *actualQuery;
	Z_PQSPeriod *targetStatedPeriod;
	char *expiration; /* OPT */
	Z_InternationalString *resultSetPackage; /* OPT */
	char *lastQueryTime;
	Odr_int *lastResultNumber;
	Odr_int *numberSinceModify; /* OPT */
};

struct Z_PQSPeriod {
	int which;
	union {
		Z_IntUnit *unit;
		Odr_null *businessDaily;
		Odr_null *continuous;
		Z_InternationalString *other;
#define Z_PQSPeriod_unit 1
#define Z_PQSPeriod_businessDaily 2
#define Z_PQSPeriod_continuous 3
#define Z_PQSPeriod_other 4
	} u;
};

#ifdef __cplusplus
}
#endif
#endif
