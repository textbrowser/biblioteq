/** \file z-univ.h
    \brief ASN.1 Module ResourceReport-Format-Universe-1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_univ_H
#define z_univ_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_UniverseReportHits Z_UniverseReportHits;
YAZ_EXPORT int z_UniverseReportHits(ODR o, Z_UniverseReportHits **p, int opt, const char *name);

typedef struct Z_UniverseReportDuplicate Z_UniverseReportDuplicate;
YAZ_EXPORT int z_UniverseReportDuplicate(ODR o, Z_UniverseReportDuplicate **p, int opt, const char *name);

typedef struct Z_UniverseReport Z_UniverseReport;
YAZ_EXPORT int z_UniverseReport(ODR o, Z_UniverseReport **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_UniverseReportHits {
	Z_StringOrNumeric *database;
	Z_StringOrNumeric *hits;
};

struct Z_UniverseReportDuplicate {
	Z_StringOrNumeric *hitno;
};

struct Z_UniverseReport {
	Odr_int *totalHits;
	int which;
	union {
		Z_UniverseReportHits *databaseHits;
		Z_UniverseReportDuplicate *duplicate;
#define Z_UniverseReport_databaseHits 1
#define Z_UniverseReport_duplicate 2
	} u;
};

#ifdef __cplusplus
}
#endif
#endif
