/** \file z-oclcui.h
    \brief ASN.1 Module UserInfoFormat-oclcUserInformation

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_oclcui_H
#define z_oclcui_H

#include <yaz/odr.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_OCLC_UserInformation Z_OCLC_UserInformation;
YAZ_EXPORT int z_OCLC_UserInformation(ODR o, Z_OCLC_UserInformation **p, int opt, const char *name);

typedef char  Z_DBName;
YAZ_EXPORT int z_DBName(ODR o, Z_DBName **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_OCLC_UserInformation {
	char *motd; /* OPT */
	int num_dblist;
	Z_DBName **dblist; /* OPT */
	Odr_bool *failReason; /* OPT */
	Odr_int *code; /* OPT */
	char *text; /* OPT */
};



#ifdef __cplusplus
}
#endif
#endif
