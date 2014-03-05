/** \file z-sum.h
    \brief ASN.1 Module RecordSyntax-summary

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_sum_H
#define z_sum_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_BriefBib Z_BriefBib;
YAZ_EXPORT int z_BriefBib(ODR o, Z_BriefBib **p, int opt, const char *name);

typedef struct Z_FormatSpec Z_FormatSpec;
YAZ_EXPORT int z_FormatSpec(ODR o, Z_FormatSpec **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_BriefBib {
	Z_InternationalString *title;
	Z_InternationalString *author; /* OPT */
	Z_InternationalString *callNumber; /* OPT */
	Z_InternationalString *recordType; /* OPT */
	Z_InternationalString *bibliographicLevel; /* OPT */
	int num_format;
	Z_FormatSpec **format; /* OPT */
	Z_InternationalString *publicationPlace; /* OPT */
	Z_InternationalString *publicationDate; /* OPT */
	Z_InternationalString *targetSystemKey; /* OPT */
	Z_InternationalString *satisfyingElement; /* OPT */
	Odr_int *rank; /* OPT */
	Z_InternationalString *documentId; /* OPT */
	Z_InternationalString *abstract; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_FormatSpec {
	Z_InternationalString *type;
	Odr_int *size; /* OPT */
	Odr_int *bestPosn; /* OPT */
};

#ifdef __cplusplus
}
#endif
#endif
