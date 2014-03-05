/** \file zes-exps.h
    \brief ASN.1 Module ESFormat-ExportSpecification

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef zes_exps_H
#define zes_exps_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_ESExportSpecificationEsRequest Z_ESExportSpecificationEsRequest;
YAZ_EXPORT int z_ESExportSpecificationEsRequest(ODR o, Z_ESExportSpecificationEsRequest **p, int opt, const char *name);

typedef struct Z_ESExportSpecificationTaskPackage Z_ESExportSpecificationTaskPackage;
YAZ_EXPORT int z_ESExportSpecificationTaskPackage(ODR o, Z_ESExportSpecificationTaskPackage **p, int opt, const char *name);

typedef struct Z_ESExportSpecification Z_ESExportSpecification;
YAZ_EXPORT int z_ESExportSpecification(ODR o, Z_ESExportSpecification **p, int opt, const char *name);

typedef struct Z_ESOriginPartToKeep Z_ESOriginPartToKeep;
YAZ_EXPORT int z_ESOriginPartToKeep(ODR o, Z_ESOriginPartToKeep **p, int opt, const char *name);

typedef struct Z_ESDestinationOther Z_ESDestinationOther;
YAZ_EXPORT int z_ESDestinationOther(ODR o, Z_ESDestinationOther **p, int opt, const char *name);

typedef struct Z_ESDestination Z_ESDestination;
YAZ_EXPORT int z_ESDestination(ODR o, Z_ESDestination **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_ESExportSpecificationEsRequest {
	Z_ESOriginPartToKeep *toKeep;
	Odr_null *notToKeep;
};

struct Z_ESExportSpecificationTaskPackage {
	Z_ESOriginPartToKeep *originPart;
	Odr_null *targetPart;
};

struct Z_ESExportSpecification {
	int which;
	union {
		Z_ESExportSpecificationEsRequest *esRequest;
		Z_ESExportSpecificationTaskPackage *taskPackage;
#define Z_ESExportSpecification_esRequest 1
#define Z_ESExportSpecification_taskPackage 2
	} u;
};

struct Z_ESOriginPartToKeep {
	Z_CompSpec *composition;
	Z_ESDestination *exportDestination;
};

struct Z_ESDestinationOther {
	Z_InternationalString *vehicle; /* OPT */
	Z_InternationalString *destination;
};

struct Z_ESDestination {
	int which;
	union {
		Z_InternationalString *phoneNumber;
		Z_InternationalString *faxNumber;
		Z_InternationalString *x400address;
		Z_InternationalString *emailAddress;
		Z_InternationalString *pagerNumber;
		Z_InternationalString *ftpAddress;
		Z_InternationalString *ftamAddress;
		Z_InternationalString *printerAddress;
		Z_ESDestinationOther *other;
#define Z_ESDestination_phoneNumber 1
#define Z_ESDestination_faxNumber 2
#define Z_ESDestination_x400address 3
#define Z_ESDestination_emailAddress 4
#define Z_ESDestination_pagerNumber 5
#define Z_ESDestination_ftpAddress 6
#define Z_ESDestination_ftamAddress 7
#define Z_ESDestination_printerAddress 8
#define Z_ESDestination_other 9
	} u;
};

#ifdef __cplusplus
}
#endif
#endif
