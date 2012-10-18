/** \file z-mterm2.h
    \brief ASN.1 Module UserInfoFormat-multipleSearchTerms-2

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_mterm2_H
#define z_mterm2_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_MultipleSearchTerms_2_s Z_MultipleSearchTerms_2_s;
YAZ_EXPORT int z_MultipleSearchTerms_2_s(ODR o, Z_MultipleSearchTerms_2_s **p, int opt, const char *name);

typedef struct Z_MultipleSearchTerms_2 Z_MultipleSearchTerms_2;
YAZ_EXPORT int z_MultipleSearchTerms_2(ODR o, Z_MultipleSearchTerms_2 **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_MultipleSearchTerms_2_s {
	Z_Term *term;
	Odr_bool *flag; /* OPT */
};

struct Z_MultipleSearchTerms_2 {
	int num;
	Z_MultipleSearchTerms_2_s **elements;
};

#ifdef __cplusplus
}
#endif
#endif
