/** \file z-accdes1.h
    \brief ASN.1 Module AccessControlFormat-des-1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_accdes1_H
#define z_accdes1_H

#include <yaz/odr.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_DES_RN_Object Z_DES_RN_Object;
YAZ_EXPORT int z_DES_RN_Object(ODR o, Z_DES_RN_Object **p, int opt, const char *name);

typedef struct Z_DRNType Z_DRNType;
YAZ_EXPORT int z_DRNType(ODR o, Z_DRNType **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_DES_RN_Object {
	int which;
	union {
		Z_DRNType *challenge;
		Z_DRNType *response;
#define Z_DES_RN_Object_challenge 1
#define Z_DES_RN_Object_response 2
	} u;
};

struct Z_DRNType {
	Odr_oct *userId; /* OPT */
	Odr_oct *salt; /* OPT */
	Odr_oct *randomNumber;
};

#ifdef __cplusplus
}
#endif
#endif
