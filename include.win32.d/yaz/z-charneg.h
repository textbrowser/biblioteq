/** \file z-charneg.h
    \brief ASN.1 Module NegotiationRecordDefinition-charSetandLanguageNegotiation-3

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_charneg_H
#define z_charneg_H

#include <yaz/odr.h>
#include <yaz/z-exp.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_CharSetandLanguageNegotiation Z_CharSetandLanguageNegotiation;
YAZ_EXPORT int z_CharSetandLanguageNegotiation(ODR o, Z_CharSetandLanguageNegotiation **p, int opt, const char *name);

typedef struct Z_OriginProposal_0 Z_OriginProposal_0;
YAZ_EXPORT int z_OriginProposal_0(ODR o, Z_OriginProposal_0 **p, int opt, const char *name);

typedef struct Z_OriginProposal Z_OriginProposal;
YAZ_EXPORT int z_OriginProposal(ODR o, Z_OriginProposal **p, int opt, const char *name);

typedef struct Z_TargetResponse Z_TargetResponse;
YAZ_EXPORT int z_TargetResponse(ODR o, Z_TargetResponse **p, int opt, const char *name);

typedef struct Z_PrivateCharacterSetViaOid Z_PrivateCharacterSetViaOid;
YAZ_EXPORT int z_PrivateCharacterSetViaOid(ODR o, Z_PrivateCharacterSetViaOid **p, int opt, const char *name);

typedef struct Z_PrivateCharacterSet Z_PrivateCharacterSet;
YAZ_EXPORT int z_PrivateCharacterSet(ODR o, Z_PrivateCharacterSet **p, int opt, const char *name);

typedef struct Z_Iso2022OriginProposal Z_Iso2022OriginProposal;
YAZ_EXPORT int z_Iso2022OriginProposal(ODR o, Z_Iso2022OriginProposal **p, int opt, const char *name);

typedef struct Z_Iso2022TargetResponse Z_Iso2022TargetResponse;
YAZ_EXPORT int z_Iso2022TargetResponse(ODR o, Z_Iso2022TargetResponse **p, int opt, const char *name);

typedef struct Z_Iso2022 Z_Iso2022;
YAZ_EXPORT int z_Iso2022(ODR o, Z_Iso2022 **p, int opt, const char *name);

typedef struct Z_Environment Z_Environment;
YAZ_EXPORT int z_Environment(ODR o, Z_Environment **p, int opt, const char *name);

typedef struct Z_InitialSet Z_InitialSet;
YAZ_EXPORT int z_InitialSet(ODR o, Z_InitialSet **p, int opt, const char *name);

typedef struct Z_LeftAndRight Z_LeftAndRight;
YAZ_EXPORT int z_LeftAndRight(ODR o, Z_LeftAndRight **p, int opt, const char *name);

typedef struct Z_Iso10646 Z_Iso10646;
YAZ_EXPORT int z_Iso10646(ODR o, Z_Iso10646 **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_CharSetandLanguageNegotiation {
	int which;
	union {
		Z_OriginProposal *proposal;
		Z_TargetResponse *response;
#define Z_CharSetandLanguageNegotiation_proposal 1
#define Z_CharSetandLanguageNegotiation_response 2
	} u;
};

struct Z_OriginProposal_0 {
	int which;
	union {
		Z_Iso2022 *iso2022;
		Z_Iso10646 *iso10646;
		Z_PrivateCharacterSet *zprivate;
#define Z_OriginProposal_0_iso2022 1
#define Z_OriginProposal_0_iso10646 2
#define Z_OriginProposal_0_private 3
	} u;
};

struct Z_OriginProposal {
	int num_proposedCharSets;
	Z_OriginProposal_0 **proposedCharSets; /* OPT */
	int num_proposedlanguages;
	Z_LanguageCode **proposedlanguages; /* OPT */
	Odr_bool *recordsInSelectedCharSets; /* OPT */
};

struct Z_TargetResponse {
	int which;
	union {
		Z_Iso2022 *iso2022;
		Z_Iso10646 *iso10646;
		Z_PrivateCharacterSet *zprivate;
		Odr_null *none;
#define Z_TargetResponse_iso2022 1
#define Z_TargetResponse_iso10646 2
#define Z_TargetResponse_private 3
#define Z_TargetResponse_none 4
	} u; /* OPT */
	Z_LanguageCode *selectedLanguage; /* OPT */
	Odr_bool *recordsInSelectedCharSets; /* OPT */
};

struct Z_PrivateCharacterSetViaOid {
	int num;
	Odr_oid **elements;
};

struct Z_PrivateCharacterSet {
	int which;
	union {
		Z_PrivateCharacterSetViaOid *viaOid;
		Z_External *externallySpecified;
		Odr_null *previouslyAgreedUpon;
#define Z_PrivateCharacterSet_viaOid 1
#define Z_PrivateCharacterSet_externallySpecified 2
#define Z_PrivateCharacterSet_previouslyAgreedUpon 3
	} u;
};

struct Z_Iso2022OriginProposal {
	Z_Environment *proposedEnvironment; /* OPT */
	int num_proposedSets;
	Odr_int **proposedSets;
	int num_proposedInitialSets;
	Z_InitialSet **proposedInitialSets;
	Z_LeftAndRight *proposedLeftAndRight;
};

struct Z_Iso2022TargetResponse {
	Z_Environment *selectedEnvironment;
	int num_selectedSets;
	Odr_int **selectedSets;
	Z_InitialSet *selectedinitialSet;
	Z_LeftAndRight *selectedLeftAndRight;
};

struct Z_Iso2022 {
	int which;
	union {
		Z_Iso2022OriginProposal *originProposal;
		Z_Iso2022TargetResponse *targetResponse;
#define Z_Iso2022_originProposal 1
#define Z_Iso2022_targetResponse 2
	} u;
};

struct Z_Environment {
	int which;
	union {
		Odr_null *sevenBit;
		Odr_null *eightBit;
#define Z_Environment_sevenBit 1
#define Z_Environment_eightBit 2
	} u;
};

struct Z_InitialSet {
	Odr_int *g0; /* OPT */
	Odr_int *g1; /* OPT */
	Odr_int *g2; /* OPT */
	Odr_int *g3; /* OPT */
	Odr_int *c0;
	Odr_int *c1; /* OPT */
};

struct Z_LeftAndRight {
#define Z_LeftAndRight_g0 0
#define Z_LeftAndRight_g1 1
#define Z_LeftAndRight_g2 2
#define Z_LeftAndRight_g3 3
	Odr_int *gLeft;
#define Z_LeftAndRight_g1 1
#define Z_LeftAndRight_g2 2
#define Z_LeftAndRight_g3 3
	Odr_int *gRight; /* OPT */
};

struct Z_Iso10646 {
	Odr_oid *collections; /* OPT */
	Odr_oid *encodingLevel;
};

#ifdef __cplusplus
}
#endif
#endif
