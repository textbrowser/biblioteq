/** \file z-uifr1.h
    \brief ASN.1 Module UserInfoFormat-searchResult-1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_uifr1_H
#define z_uifr1_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_SearchInfoReport_s Z_SearchInfoReport_s;
YAZ_EXPORT int z_SearchInfoReport_s(ODR o, Z_SearchInfoReport_s **p, int opt, const char *name);

typedef struct Z_SearchInfoReport Z_SearchInfoReport;
YAZ_EXPORT int z_SearchInfoReport(ODR o, Z_SearchInfoReport **p, int opt, const char *name);

typedef struct Z_ResultsByDB_sList Z_ResultsByDB_sList;
YAZ_EXPORT int z_ResultsByDB_sList(ODR o, Z_ResultsByDB_sList **p, int opt, const char *name);

typedef struct Z_ResultsByDB_s Z_ResultsByDB_s;
YAZ_EXPORT int z_ResultsByDB_s(ODR o, Z_ResultsByDB_s **p, int opt, const char *name);

typedef struct Z_ResultsByDB Z_ResultsByDB;
YAZ_EXPORT int z_ResultsByDB(ODR o, Z_ResultsByDB **p, int opt, const char *name);

typedef struct Z_QueryExpressionTerm Z_QueryExpressionTerm;
YAZ_EXPORT int z_QueryExpressionTerm(ODR o, Z_QueryExpressionTerm **p, int opt, const char *name);

typedef struct Z_QueryExpression Z_QueryExpression;
YAZ_EXPORT int z_QueryExpression(ODR o, Z_QueryExpression **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_SearchInfoReport_s {
	Z_InternationalString *subqueryId; /* OPT */
	Odr_bool *fullQuery;
	Z_QueryExpression *subqueryExpression; /* OPT */
	Z_QueryExpression *subqueryInterpretation; /* OPT */
	Z_QueryExpression *subqueryRecommendation; /* OPT */
	Odr_int *subqueryCount; /* OPT */
	Z_IntUnit *subqueryWeight; /* OPT */
	Z_ResultsByDB *resultsByDB; /* OPT */
};

struct Z_SearchInfoReport {
	int num;
	Z_SearchInfoReport_s **elements;
};

struct Z_ResultsByDB_sList {
	int num;
	Z_DatabaseName **elements;
};

struct Z_ResultsByDB_s {
	int which;
	union {
		Odr_null *all;
		Z_ResultsByDB_sList *list;
#define Z_ResultsByDB_s_all 1
#define Z_ResultsByDB_s_list 2
	} u;
	Odr_int *count; /* OPT */
	Z_InternationalString *resultSetName; /* OPT */
};

struct Z_ResultsByDB {
	int num;
	Z_ResultsByDB_s **elements;
};

struct Z_QueryExpressionTerm {
	Z_Term *queryTerm;
	Z_InternationalString *termComment; /* OPT */
};

struct Z_QueryExpression {
	int which;
	union {
		Z_QueryExpressionTerm *term;
		Z_Query *query;
#define Z_QueryExpression_term 1
#define Z_QueryExpression_query 2
	} u;
};

#ifdef __cplusplus
}
#endif
#endif
