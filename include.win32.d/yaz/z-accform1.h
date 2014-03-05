/** \file z-accform1.h
    \brief ASN.1 Module AccessControlFormat-prompt-1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_accform1_H
#define z_accform1_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_PromptObject1 Z_PromptObject1;
YAZ_EXPORT int z_PromptObject1(ODR o, Z_PromptObject1 **p, int opt, const char *name);

typedef struct Z_ChallengeUnit1 Z_ChallengeUnit1;
YAZ_EXPORT int z_ChallengeUnit1(ODR o, Z_ChallengeUnit1 **p, int opt, const char *name);

typedef struct Z_Challenge1 Z_Challenge1;
YAZ_EXPORT int z_Challenge1(ODR o, Z_Challenge1 **p, int opt, const char *name);

typedef struct Z_ResponseUnit1 Z_ResponseUnit1;
YAZ_EXPORT int z_ResponseUnit1(ODR o, Z_ResponseUnit1 **p, int opt, const char *name);

typedef struct Z_Response1 Z_Response1;
YAZ_EXPORT int z_Response1(ODR o, Z_Response1 **p, int opt, const char *name);

typedef struct Z_PromptIdEnumeratedPrompt Z_PromptIdEnumeratedPrompt;
YAZ_EXPORT int z_PromptIdEnumeratedPrompt(ODR o, Z_PromptIdEnumeratedPrompt **p, int opt, const char *name);

typedef struct Z_PromptId Z_PromptId;
YAZ_EXPORT int z_PromptId(ODR o, Z_PromptId **p, int opt, const char *name);

typedef struct Z_Encryption Z_Encryption;
YAZ_EXPORT int z_Encryption(ODR o, Z_Encryption **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_PromptObject1 {
	int which;
	union {
		Z_Challenge1 *challenge;
		Z_Response1 *response;
#define Z_PromptObject1_challenge 1
#define Z_PromptObject1_response 2
	} u;
};

struct Z_ChallengeUnit1 {
	Z_PromptId *promptId;
	Z_InternationalString *defaultResponse; /* OPT */
	int which;
	union {
		Z_InternationalString *character;
		Z_Encryption *encrypted;
#define Z_ChallengeUnit1_character 1
#define Z_ChallengeUnit1_encrypted 2
	} u; /* OPT */
	Z_InternationalString *regExpr; /* OPT */
	Odr_null *responseRequired; /* OPT */
	int num_allowedValues;
	Z_InternationalString **allowedValues; /* OPT */
	Odr_null *shouldSave; /* OPT */
#define Z_ChallengeUnit1_integer 1
#define Z_ChallengeUnit1_date 2
#define Z_ChallengeUnit1_float 3
#define Z_ChallengeUnit1_alphaNumeric 4
#define Z_ChallengeUnit1_url_urn 5
#define Z_ChallengeUnit1_boolean 6
	Odr_int *dataType; /* OPT */
	Z_External *diagnostic; /* OPT */
};

struct Z_Challenge1 {
	int num;
	Z_ChallengeUnit1 **elements;
};

struct Z_ResponseUnit1 {
	Z_PromptId *promptId;
	int which;
	union {
		Z_InternationalString *string;
		Odr_bool *accept;
		Odr_null *acknowledge;
		Z_DiagRec *diagnostic;
		Z_Encryption *encrypted;
#define Z_ResponseUnit1_string 1
#define Z_ResponseUnit1_accept 2
#define Z_ResponseUnit1_acknowledge 3
#define Z_ResponseUnit1_diagnostic 4
#define Z_ResponseUnit1_encrypted 5
	} u;
};

struct Z_Response1 {
	int num;
	Z_ResponseUnit1 **elements;
};

struct Z_PromptIdEnumeratedPrompt {
#define Z_PromptIdEnumeratedPrompt_groupId 0
#define Z_PromptIdEnumeratedPrompt_userId 1
#define Z_PromptIdEnumeratedPrompt_password 2
#define Z_PromptIdEnumeratedPrompt_newPassword 3
#define Z_PromptIdEnumeratedPrompt_copyright 4
#define Z_PromptIdEnumeratedPrompt_sessionId 5
	Odr_int *type;
	Z_InternationalString *suggestedString; /* OPT */
};

struct Z_PromptId {
	int which;
	union {
		Z_PromptIdEnumeratedPrompt *enumeratedPrompt;
		Z_InternationalString *nonEnumeratedPrompt;
#define Z_PromptId_enumeratedPrompt 1
#define Z_PromptId_nonEnumeratedPrompt 2
	} u;
};

struct Z_Encryption {
	Odr_oct *cryptType; /* OPT */
	Odr_oct *credential; /* OPT */
	Odr_oct *data;
};

#ifdef __cplusplus
}
#endif
#endif
