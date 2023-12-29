/** \file z-rrf1.h
    \brief ASN.1 Module ResourceReport-Format-Resource-1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_rrf1_H
#define z_rrf1_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_ResourceReport1 Z_ResourceReport1;
YAZ_EXPORT int z_ResourceReport1(ODR o, Z_ResourceReport1 **p, int opt, const char *name);

typedef struct Z_Estimate1 Z_Estimate1;
YAZ_EXPORT int z_Estimate1(ODR o, Z_Estimate1 **p, int opt, const char *name);

typedef Odr_int  Z_EstimateType;
YAZ_EXPORT int z_EstimateType(ODR o, Z_EstimateType **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_ResourceReport1 {
	int num_estimates;
	Z_Estimate1 **estimates;
	Z_InternationalString *message;
};

struct Z_Estimate1 {
	Z_EstimateType *type;
	Odr_int *value;
	Odr_int *currency_code; /* OPT */
};

#define Z_EstimateType_currentSearchRecords 1
#define Z_EstimateType_finalSearchRecords 2
#define Z_EstimateType_currentPresentRecords 3
#define Z_EstimateType_finalPresentRecords 4
#define Z_EstimateType_currentOpTimeProcessing 5
#define Z_EstimateType_finalOpTimeProcessing 6
#define Z_EstimateType_currentAssocTime 7
#define Z_EstimateType_currentOperationCost 8
#define Z_EstimateType_finalOperationCost 9
#define Z_EstimateType_currentAssocCost 10
#define Z_EstimateType_finalOpTimeElapsed 11
#define Z_EstimateType_percentComplete 12
#define Z_EstimateType_currentSearchAssocCost 13
#define Z_EstimateType_currentPresentAssocCost 14
#define Z_EstimateType_currentConnectAssocCost 15
#define Z_EstimateType_currentOtherAssocCost 16

#ifdef __cplusplus
}
#endif
#endif
