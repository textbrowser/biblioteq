/** \file z-opac.h
    \brief ASN.1 Module RecordSyntax-opac

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_opac_H
#define z_opac_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_OPACRecord Z_OPACRecord;
YAZ_EXPORT int z_OPACRecord(ODR o, Z_OPACRecord **p, int opt, const char *name);

typedef struct Z_HoldingsRecord Z_HoldingsRecord;
YAZ_EXPORT int z_HoldingsRecord(ODR o, Z_HoldingsRecord **p, int opt, const char *name);

typedef struct Z_HoldingsAndCircData Z_HoldingsAndCircData;
YAZ_EXPORT int z_HoldingsAndCircData(ODR o, Z_HoldingsAndCircData **p, int opt, const char *name);

typedef struct Z_Volume Z_Volume;
YAZ_EXPORT int z_Volume(ODR o, Z_Volume **p, int opt, const char *name);

typedef struct Z_CircRecord Z_CircRecord;
YAZ_EXPORT int z_CircRecord(ODR o, Z_CircRecord **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_OPACRecord {
	Z_External *bibliographicRecord; /* OPT */
	int num_holdingsData;
	Z_HoldingsRecord **holdingsData; /* OPT */
};

struct Z_HoldingsRecord {
	int which;
	union {
		Z_External *marcHoldingsRecord;
		Z_HoldingsAndCircData *holdingsAndCirc;
#define Z_HoldingsRecord_marcHoldingsRecord 1
#define Z_HoldingsRecord_holdingsAndCirc 2
	} u;
};

struct Z_HoldingsAndCircData {
	Z_InternationalString *typeOfRecord; /* OPT */
	Z_InternationalString *encodingLevel; /* OPT */
	Z_InternationalString *format; /* OPT */
	Z_InternationalString *receiptAcqStatus; /* OPT */
	Z_InternationalString *generalRetention; /* OPT */
	Z_InternationalString *completeness; /* OPT */
	Z_InternationalString *dateOfReport; /* OPT */
	Z_InternationalString *nucCode; /* OPT */
	Z_InternationalString *localLocation; /* OPT */
	Z_InternationalString *shelvingLocation; /* OPT */
	Z_InternationalString *callNumber; /* OPT */
	Z_InternationalString *shelvingData; /* OPT */
	Z_InternationalString *copyNumber; /* OPT */
	Z_InternationalString *publicNote; /* OPT */
	Z_InternationalString *reproductionNote; /* OPT */
	Z_InternationalString *termsUseRepro; /* OPT */
	Z_InternationalString *enumAndChron; /* OPT */
	int num_volumes;
	Z_Volume **volumes; /* OPT */
	int num_circulationData;
	Z_CircRecord **circulationData; /* OPT */
};

struct Z_Volume {
	Z_InternationalString *enumeration; /* OPT */
	Z_InternationalString *chronology; /* OPT */
	Z_InternationalString *enumAndChron; /* OPT */
};

struct Z_CircRecord {
	Odr_bool *availableNow;
	Z_InternationalString *availablityDate; /* OPT */
	Z_InternationalString *availableThru; /* OPT */
	Z_InternationalString *restrictions; /* OPT */
	Z_InternationalString *itemId; /* OPT */
	Odr_bool *renewable;
	Odr_bool *onHold;
	Z_InternationalString *enumAndChron; /* OPT */
	Z_InternationalString *midspine; /* OPT */
	Z_InternationalString *temporaryLocation; /* OPT */
};

#ifdef __cplusplus
}
#endif
#endif
