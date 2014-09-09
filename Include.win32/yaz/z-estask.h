/** \file z-estask.h
    \brief ASN.1 Module RecordSyntax-ESTaskPackage

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_estask_H
#define z_estask_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_TaskPackage Z_TaskPackage;
YAZ_EXPORT int z_TaskPackage(ODR o, Z_TaskPackage **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_TaskPackage {
	Odr_oid *packageType;
	Z_InternationalString *packageName; /* OPT */
	Z_InternationalString *userId; /* OPT */
	Z_IntUnit *retentionTime; /* OPT */
	Z_Permissions *permissions; /* OPT */
	Z_InternationalString *description; /* OPT */
	Odr_oct *targetReference; /* OPT */
	char *creationDateTime; /* OPT */
#define Z_TaskPackage_pending 0
#define Z_TaskPackage_active 1
#define Z_TaskPackage_complete 2
#define Z_TaskPackage_aborted 3
	Odr_int *taskStatus;
	int num_packageDiagnostics;
	Z_DiagRec **packageDiagnostics; /* OPT */
	Z_External *taskSpecificParameters;
};

#ifdef __cplusplus
}
#endif
#endif
