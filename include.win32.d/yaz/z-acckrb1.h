/** \file z-acckrb1.h
    \brief ASN.1 Module AccessControlFormat-krb-1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_acckrb1_H
#define z_acckrb1_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_KRBObject Z_KRBObject;
YAZ_EXPORT int z_KRBObject(ODR o, Z_KRBObject **p, int opt, const char *name);

typedef struct Z_KRBRequest Z_KRBRequest;
YAZ_EXPORT int z_KRBRequest(ODR o, Z_KRBRequest **p, int opt, const char *name);

typedef struct Z_KRBResponse Z_KRBResponse;
YAZ_EXPORT int z_KRBResponse(ODR o, Z_KRBResponse **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_KRBObject {
	int which;
	union {
		Z_KRBRequest *challenge;
		Z_KRBResponse *response;
#define Z_KRBObject_challenge 1
#define Z_KRBObject_response 2
	} u;
};

struct Z_KRBRequest {
	Z_InternationalString *service;
	Z_InternationalString *instance; /* OPT */
	Z_InternationalString *realm; /* OPT */
};

struct Z_KRBResponse {
	Z_InternationalString *userid; /* OPT */
	Odr_oct *ticket;
};

#ifdef __cplusplus
}
#endif
#endif
