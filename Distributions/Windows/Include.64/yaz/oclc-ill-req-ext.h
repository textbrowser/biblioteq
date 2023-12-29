/** \file oclc-ill-req-ext.h
    \brief ASN.1 Module OCLCILLRequestExtension

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef oclc_ill_req_ext_H
#define oclc_ill_req_ext_H

#include <yaz/odr.h>
#include <yaz/ill-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct ILL_OCLCILLRequestExtension ILL_OCLCILLRequestExtension;
YAZ_EXPORT int ill_OCLCILLRequestExtension(ODR o, ILL_OCLCILLRequestExtension **p, int opt, const char *name);

typedef struct ILL_SystemNo_s ILL_SystemNo_s;
YAZ_EXPORT int ill_SystemNo_s(ODR o, ILL_SystemNo_s **p, int opt, const char *name);

typedef struct ILL_SystemNo ILL_SystemNo;
YAZ_EXPORT int ill_SystemNo(ODR o, ILL_SystemNo **p, int opt, const char *name);

typedef Odr_int  ILL_System;
YAZ_EXPORT int ill_System(ODR o, ILL_System **p, int opt, const char *name);

typedef ILL_String  ILL_RecordNo;
YAZ_EXPORT int ill_RecordNo(ODR o, ILL_RecordNo **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct ILL_OCLCILLRequestExtension {
	ILL_String *clientDepartment; /* OPT */
	ILL_String *paymentMethod; /* OPT */
	ILL_String *uniformTitle; /* OPT */
	ILL_String *dissertation; /* OPT */
	ILL_String *issueNumber; /* OPT */
	ILL_String *volume; /* OPT */
	ILL_String *affiliations; /* OPT */
	ILL_String *source; /* OPT */
};

struct ILL_SystemNo_s {
	ILL_System *System;
	ILL_RecordNo *recordNo;
};

struct ILL_SystemNo {
	int num;
	ILL_SystemNo_s **elements;
};

#define ILL_System_dobis 1
#define ILL_System_candoc 2
#define ILL_System_oclc 3
#define ILL_System_rlin 4
#define ILL_System_utlas 5
#define ILL_System_other 6



#ifdef __cplusplus
}
#endif
#endif
