/** \file z-core.h
    \brief ASN.1 Module Z39-50-APDU-1995

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_core_H
#define z_core_H

#include <yaz/odr.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_External Z_External;
YAZ_EXPORT int z_External(ODR o, Z_External **p, int opt, const char *name);


typedef struct Z_APDU Z_APDU;
YAZ_EXPORT int z_APDU(ODR o, Z_APDU **p, int opt, const char *name);

typedef struct Z_InitRequest Z_InitRequest;
YAZ_EXPORT int z_InitRequest(ODR o, Z_InitRequest **p, int opt, const char *name);

typedef struct Z_IdPass Z_IdPass;
YAZ_EXPORT int z_IdPass(ODR o, Z_IdPass **p, int opt, const char *name);

typedef struct Z_IdAuthentication Z_IdAuthentication;
YAZ_EXPORT int z_IdAuthentication(ODR o, Z_IdAuthentication **p, int opt, const char *name);

typedef struct Z_InitResponse Z_InitResponse;
YAZ_EXPORT int z_InitResponse(ODR o, Z_InitResponse **p, int opt, const char *name);

typedef Odr_bitmask  Z_ProtocolVersion;
YAZ_EXPORT int z_ProtocolVersion(ODR o, Z_ProtocolVersion **p, int opt, const char *name);

typedef Odr_bitmask  Z_Options;
YAZ_EXPORT int z_Options(ODR o, Z_Options **p, int opt, const char *name);

typedef struct Z_SearchRequest Z_SearchRequest;
YAZ_EXPORT int z_SearchRequest(ODR o, Z_SearchRequest **p, int opt, const char *name);

typedef struct Z_Query Z_Query;
YAZ_EXPORT int z_Query(ODR o, Z_Query **p, int opt, const char *name);

typedef struct Z_RPNQuery Z_RPNQuery;
YAZ_EXPORT int z_RPNQuery(ODR o, Z_RPNQuery **p, int opt, const char *name);

typedef struct Z_Complex Z_Complex;
YAZ_EXPORT int z_Complex(ODR o, Z_Complex **p, int opt, const char *name);

typedef struct Z_RPNStructure Z_RPNStructure;
YAZ_EXPORT int z_RPNStructure(ODR o, Z_RPNStructure **p, int opt, const char *name);

typedef struct Z_Operand Z_Operand;
YAZ_EXPORT int z_Operand(ODR o, Z_Operand **p, int opt, const char *name);

typedef struct Z_AttributesPlusTerm Z_AttributesPlusTerm;
YAZ_EXPORT int z_AttributesPlusTerm(ODR o, Z_AttributesPlusTerm **p, int opt, const char *name);

typedef struct Z_ResultSetPlusAttributes Z_ResultSetPlusAttributes;
YAZ_EXPORT int z_ResultSetPlusAttributes(ODR o, Z_ResultSetPlusAttributes **p, int opt, const char *name);

typedef struct Z_AttributeList Z_AttributeList;
YAZ_EXPORT int z_AttributeList(ODR o, Z_AttributeList **p, int opt, const char *name);

typedef struct Z_Term Z_Term;
YAZ_EXPORT int z_Term(ODR o, Z_Term **p, int opt, const char *name);

typedef struct Z_Operator Z_Operator;
YAZ_EXPORT int z_Operator(ODR o, Z_Operator **p, int opt, const char *name);

typedef struct Z_ComplexAttribute Z_ComplexAttribute;
YAZ_EXPORT int z_ComplexAttribute(ODR o, Z_ComplexAttribute **p, int opt, const char *name);

typedef struct Z_AttributeElement Z_AttributeElement;
YAZ_EXPORT int z_AttributeElement(ODR o, Z_AttributeElement **p, int opt, const char *name);

typedef struct Z_ProximityOperator Z_ProximityOperator;
YAZ_EXPORT int z_ProximityOperator(ODR o, Z_ProximityOperator **p, int opt, const char *name);

typedef Odr_int  Z_ProxUnit;
YAZ_EXPORT int z_ProxUnit(ODR o, Z_ProxUnit **p, int opt, const char *name);

typedef struct Z_SearchResponse Z_SearchResponse;
YAZ_EXPORT int z_SearchResponse(ODR o, Z_SearchResponse **p, int opt, const char *name);

typedef struct Z_RecordComposition Z_RecordComposition;
YAZ_EXPORT int z_RecordComposition(ODR o, Z_RecordComposition **p, int opt, const char *name);

typedef struct Z_PresentRequest Z_PresentRequest;
YAZ_EXPORT int z_PresentRequest(ODR o, Z_PresentRequest **p, int opt, const char *name);

typedef struct Z_Segment Z_Segment;
YAZ_EXPORT int z_Segment(ODR o, Z_Segment **p, int opt, const char *name);

typedef struct Z_PresentResponse Z_PresentResponse;
YAZ_EXPORT int z_PresentResponse(ODR o, Z_PresentResponse **p, int opt, const char *name);

typedef struct Z_NamePlusRecordList Z_NamePlusRecordList;
YAZ_EXPORT int z_NamePlusRecordList(ODR o, Z_NamePlusRecordList **p, int opt, const char *name);

typedef struct Z_DiagRecs Z_DiagRecs;
YAZ_EXPORT int z_DiagRecs(ODR o, Z_DiagRecs **p, int opt, const char *name);

typedef struct Z_Records Z_Records;
YAZ_EXPORT int z_Records(ODR o, Z_Records **p, int opt, const char *name);

typedef struct Z_NamePlusRecord Z_NamePlusRecord;
YAZ_EXPORT int z_NamePlusRecord(ODR o, Z_NamePlusRecord **p, int opt, const char *name);

typedef struct Z_FragmentSyntax Z_FragmentSyntax;
YAZ_EXPORT int z_FragmentSyntax(ODR o, Z_FragmentSyntax **p, int opt, const char *name);

typedef struct Z_DiagRec Z_DiagRec;
YAZ_EXPORT int z_DiagRec(ODR o, Z_DiagRec **p, int opt, const char *name);

typedef struct Z_DefaultDiagFormat Z_DefaultDiagFormat;
YAZ_EXPORT int z_DefaultDiagFormat(ODR o, Z_DefaultDiagFormat **p, int opt, const char *name);

typedef struct Z_Range Z_Range;
YAZ_EXPORT int z_Range(ODR o, Z_Range **p, int opt, const char *name);

typedef struct Z_DatabaseSpecificUnit Z_DatabaseSpecificUnit;
YAZ_EXPORT int z_DatabaseSpecificUnit(ODR o, Z_DatabaseSpecificUnit **p, int opt, const char *name);

typedef struct Z_DatabaseSpecific Z_DatabaseSpecific;
YAZ_EXPORT int z_DatabaseSpecific(ODR o, Z_DatabaseSpecific **p, int opt, const char *name);

typedef struct Z_ElementSetNames Z_ElementSetNames;
YAZ_EXPORT int z_ElementSetNames(ODR o, Z_ElementSetNames **p, int opt, const char *name);

typedef Odr_int  Z_PresentStatus;
YAZ_EXPORT int z_PresentStatus(ODR o, Z_PresentStatus **p, int opt, const char *name);

typedef struct Z_DbSpecific Z_DbSpecific;
YAZ_EXPORT int z_DbSpecific(ODR o, Z_DbSpecific **p, int opt, const char *name);

typedef struct Z_CompSpec Z_CompSpec;
YAZ_EXPORT int z_CompSpec(ODR o, Z_CompSpec **p, int opt, const char *name);

typedef struct Z_ElementSpec Z_ElementSpec;
YAZ_EXPORT int z_ElementSpec(ODR o, Z_ElementSpec **p, int opt, const char *name);

typedef struct Z_Specification Z_Specification;
YAZ_EXPORT int z_Specification(ODR o, Z_Specification **p, int opt, const char *name);

typedef struct Z_DeleteResultSetRequest Z_DeleteResultSetRequest;
YAZ_EXPORT int z_DeleteResultSetRequest(ODR o, Z_DeleteResultSetRequest **p, int opt, const char *name);

typedef struct Z_DeleteResultSetResponse Z_DeleteResultSetResponse;
YAZ_EXPORT int z_DeleteResultSetResponse(ODR o, Z_DeleteResultSetResponse **p, int opt, const char *name);

typedef struct Z_ListStatus Z_ListStatus;
YAZ_EXPORT int z_ListStatus(ODR o, Z_ListStatus **p, int opt, const char *name);

typedef struct Z_ListStatuses Z_ListStatuses;
YAZ_EXPORT int z_ListStatuses(ODR o, Z_ListStatuses **p, int opt, const char *name);

typedef Odr_int  Z_DeleteStatus;
YAZ_EXPORT int z_DeleteStatus(ODR o, Z_DeleteStatus **p, int opt, const char *name);

typedef struct Z_AccessControlRequest Z_AccessControlRequest;
YAZ_EXPORT int z_AccessControlRequest(ODR o, Z_AccessControlRequest **p, int opt, const char *name);

typedef struct Z_AccessControlResponse Z_AccessControlResponse;
YAZ_EXPORT int z_AccessControlResponse(ODR o, Z_AccessControlResponse **p, int opt, const char *name);

typedef struct Z_ResourceControlRequest Z_ResourceControlRequest;
YAZ_EXPORT int z_ResourceControlRequest(ODR o, Z_ResourceControlRequest **p, int opt, const char *name);

typedef struct Z_ResourceControlResponse Z_ResourceControlResponse;
YAZ_EXPORT int z_ResourceControlResponse(ODR o, Z_ResourceControlResponse **p, int opt, const char *name);

typedef struct Z_TriggerResourceControlRequest Z_TriggerResourceControlRequest;
YAZ_EXPORT int z_TriggerResourceControlRequest(ODR o, Z_TriggerResourceControlRequest **p, int opt, const char *name);

typedef struct Z_ResourceReportRequest Z_ResourceReportRequest;
YAZ_EXPORT int z_ResourceReportRequest(ODR o, Z_ResourceReportRequest **p, int opt, const char *name);

typedef struct Z_ResourceReportResponse Z_ResourceReportResponse;
YAZ_EXPORT int z_ResourceReportResponse(ODR o, Z_ResourceReportResponse **p, int opt, const char *name);

typedef Z_External  Z_ResourceReport;
YAZ_EXPORT int z_ResourceReport(ODR o, Z_ResourceReport **p, int opt, const char *name);

typedef Odr_oid  Z_ResourceReportId;
YAZ_EXPORT int z_ResourceReportId(ODR o, Z_ResourceReportId **p, int opt, const char *name);

typedef struct Z_ScanRequest Z_ScanRequest;
YAZ_EXPORT int z_ScanRequest(ODR o, Z_ScanRequest **p, int opt, const char *name);

typedef struct Z_ScanResponse Z_ScanResponse;
YAZ_EXPORT int z_ScanResponse(ODR o, Z_ScanResponse **p, int opt, const char *name);

typedef struct Z_ListEntries Z_ListEntries;
YAZ_EXPORT int z_ListEntries(ODR o, Z_ListEntries **p, int opt, const char *name);

typedef struct Z_Entry Z_Entry;
YAZ_EXPORT int z_Entry(ODR o, Z_Entry **p, int opt, const char *name);

typedef struct Z_TermInfo Z_TermInfo;
YAZ_EXPORT int z_TermInfo(ODR o, Z_TermInfo **p, int opt, const char *name);

typedef struct Z_byDatabaseList_s Z_byDatabaseList_s;
YAZ_EXPORT int z_byDatabaseList_s(ODR o, Z_byDatabaseList_s **p, int opt, const char *name);

typedef struct Z_byDatabaseList Z_byDatabaseList;
YAZ_EXPORT int z_byDatabaseList(ODR o, Z_byDatabaseList **p, int opt, const char *name);

typedef struct Z_OccurrenceByAttributesElem Z_OccurrenceByAttributesElem;
YAZ_EXPORT int z_OccurrenceByAttributesElem(ODR o, Z_OccurrenceByAttributesElem **p, int opt, const char *name);

typedef struct Z_OccurrenceByAttributes Z_OccurrenceByAttributes;
YAZ_EXPORT int z_OccurrenceByAttributes(ODR o, Z_OccurrenceByAttributes **p, int opt, const char *name);

typedef struct Z_SortKeySpecList Z_SortKeySpecList;
YAZ_EXPORT int z_SortKeySpecList(ODR o, Z_SortKeySpecList **p, int opt, const char *name);

typedef struct Z_SortRequest Z_SortRequest;
YAZ_EXPORT int z_SortRequest(ODR o, Z_SortRequest **p, int opt, const char *name);

typedef struct Z_SortResponse Z_SortResponse;
YAZ_EXPORT int z_SortResponse(ODR o, Z_SortResponse **p, int opt, const char *name);

typedef struct Z_SortKeySpec Z_SortKeySpec;
YAZ_EXPORT int z_SortKeySpec(ODR o, Z_SortKeySpec **p, int opt, const char *name);

typedef struct Z_SortDbSpecificList_s Z_SortDbSpecificList_s;
YAZ_EXPORT int z_SortDbSpecificList_s(ODR o, Z_SortDbSpecificList_s **p, int opt, const char *name);

typedef struct Z_SortDbSpecificList Z_SortDbSpecificList;
YAZ_EXPORT int z_SortDbSpecificList(ODR o, Z_SortDbSpecificList **p, int opt, const char *name);

typedef struct Z_SortElement Z_SortElement;
YAZ_EXPORT int z_SortElement(ODR o, Z_SortElement **p, int opt, const char *name);

typedef struct Z_SortAttributes Z_SortAttributes;
YAZ_EXPORT int z_SortAttributes(ODR o, Z_SortAttributes **p, int opt, const char *name);

typedef struct Z_SortKey Z_SortKey;
YAZ_EXPORT int z_SortKey(ODR o, Z_SortKey **p, int opt, const char *name);

typedef struct Z_ExtendedServicesRequest Z_ExtendedServicesRequest;
YAZ_EXPORT int z_ExtendedServicesRequest(ODR o, Z_ExtendedServicesRequest **p, int opt, const char *name);

typedef struct Z_ExtendedServicesResponse Z_ExtendedServicesResponse;
YAZ_EXPORT int z_ExtendedServicesResponse(ODR o, Z_ExtendedServicesResponse **p, int opt, const char *name);

typedef struct Z_Permissions_s Z_Permissions_s;
YAZ_EXPORT int z_Permissions_s(ODR o, Z_Permissions_s **p, int opt, const char *name);

typedef struct Z_Permissions Z_Permissions;
YAZ_EXPORT int z_Permissions(ODR o, Z_Permissions **p, int opt, const char *name);

typedef struct Z_Close Z_Close;
YAZ_EXPORT int z_Close(ODR o, Z_Close **p, int opt, const char *name);

typedef Odr_int  Z_CloseReason;
YAZ_EXPORT int z_CloseReason(ODR o, Z_CloseReason **p, int opt, const char *name);

typedef struct Z_DuplicateDetectionRequest Z_DuplicateDetectionRequest;
YAZ_EXPORT int z_DuplicateDetectionRequest(ODR o, Z_DuplicateDetectionRequest **p, int opt, const char *name);

typedef struct Z_DuplicateDetectionCriterion Z_DuplicateDetectionCriterion;
YAZ_EXPORT int z_DuplicateDetectionCriterion(ODR o, Z_DuplicateDetectionCriterion **p, int opt, const char *name);

typedef struct Z_RetentionCriterion Z_RetentionCriterion;
YAZ_EXPORT int z_RetentionCriterion(ODR o, Z_RetentionCriterion **p, int opt, const char *name);

typedef struct Z_SortCriterionPreferredDatabases Z_SortCriterionPreferredDatabases;
YAZ_EXPORT int z_SortCriterionPreferredDatabases(ODR o, Z_SortCriterionPreferredDatabases **p, int opt, const char *name);

typedef struct Z_SortCriterion Z_SortCriterion;
YAZ_EXPORT int z_SortCriterion(ODR o, Z_SortCriterion **p, int opt, const char *name);

typedef struct Z_DuplicateDetectionResponse Z_DuplicateDetectionResponse;
YAZ_EXPORT int z_DuplicateDetectionResponse(ODR o, Z_DuplicateDetectionResponse **p, int opt, const char *name);

typedef Odr_oct  Z_ReferenceId;
YAZ_EXPORT int z_ReferenceId(ODR o, Z_ReferenceId **p, int opt, const char *name);

typedef Odr_oid  Z_AttributeSetId;
YAZ_EXPORT int z_AttributeSetId(ODR o, Z_AttributeSetId **p, int opt, const char *name);

typedef struct Z_OtherInformationUnit Z_OtherInformationUnit;
YAZ_EXPORT int z_OtherInformationUnit(ODR o, Z_OtherInformationUnit **p, int opt, const char *name);

typedef struct Z_OtherInformation Z_OtherInformation;
YAZ_EXPORT int z_OtherInformation(ODR o, Z_OtherInformation **p, int opt, const char *name);

typedef struct Z_InfoCategory Z_InfoCategory;
YAZ_EXPORT int z_InfoCategory(ODR o, Z_InfoCategory **p, int opt, const char *name);

typedef struct Z_IntUnit Z_IntUnit;
YAZ_EXPORT int z_IntUnit(ODR o, Z_IntUnit **p, int opt, const char *name);

typedef struct Z_Unit Z_Unit;
YAZ_EXPORT int z_Unit(ODR o, Z_Unit **p, int opt, const char *name);

typedef char  Z_InternationalString;
YAZ_EXPORT int z_InternationalString(ODR o, Z_InternationalString **p, int opt, const char *name);

typedef Z_InternationalString  Z_ResultSetId;
YAZ_EXPORT int z_ResultSetId(ODR o, Z_ResultSetId **p, int opt, const char *name);

typedef Z_InternationalString  Z_ElementSetName;
YAZ_EXPORT int z_ElementSetName(ODR o, Z_ElementSetName **p, int opt, const char *name);

typedef Z_InternationalString  Z_DatabaseName;
YAZ_EXPORT int z_DatabaseName(ODR o, Z_DatabaseName **p, int opt, const char *name);

typedef struct Z_StringOrNumeric Z_StringOrNumeric;
YAZ_EXPORT int z_StringOrNumeric(ODR o, Z_StringOrNumeric **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_APDU {
	int which;
	union {
		Z_InitRequest *initRequest;
		Z_InitResponse *initResponse;
		Z_SearchRequest *searchRequest;
		Z_SearchResponse *searchResponse;
		Z_PresentRequest *presentRequest;
		Z_PresentResponse *presentResponse;
		Z_DeleteResultSetRequest *deleteResultSetRequest;
		Z_DeleteResultSetResponse *deleteResultSetResponse;
		Z_AccessControlRequest *accessControlRequest;
		Z_AccessControlResponse *accessControlResponse;
		Z_ResourceControlRequest *resourceControlRequest;
		Z_ResourceControlResponse *resourceControlResponse;
		Z_TriggerResourceControlRequest *triggerResourceControlRequest;
		Z_ResourceReportRequest *resourceReportRequest;
		Z_ResourceReportResponse *resourceReportResponse;
		Z_ScanRequest *scanRequest;
		Z_ScanResponse *scanResponse;
		Z_SortRequest *sortRequest;
		Z_SortResponse *sortResponse;
		Z_Segment *segmentRequest;
		Z_ExtendedServicesRequest *extendedServicesRequest;
		Z_ExtendedServicesResponse *extendedServicesResponse;
		Z_Close *close;
		Z_DuplicateDetectionRequest *duplicateDetectionRequest;
		Z_DuplicateDetectionResponse *duplicateDetectionResponse;
#define Z_APDU_initRequest 1
#define Z_APDU_initResponse 2
#define Z_APDU_searchRequest 3
#define Z_APDU_searchResponse 4
#define Z_APDU_presentRequest 5
#define Z_APDU_presentResponse 6
#define Z_APDU_deleteResultSetRequest 7
#define Z_APDU_deleteResultSetResponse 8
#define Z_APDU_accessControlRequest 9
#define Z_APDU_accessControlResponse 10
#define Z_APDU_resourceControlRequest 11
#define Z_APDU_resourceControlResponse 12
#define Z_APDU_triggerResourceControlRequest 13
#define Z_APDU_resourceReportRequest 14
#define Z_APDU_resourceReportResponse 15
#define Z_APDU_scanRequest 16
#define Z_APDU_scanResponse 17
#define Z_APDU_sortRequest 18
#define Z_APDU_sortResponse 19
#define Z_APDU_segmentRequest 20
#define Z_APDU_extendedServicesRequest 21
#define Z_APDU_extendedServicesResponse 22
#define Z_APDU_close 23
#define Z_APDU_duplicateDetectionRequest 24
#define Z_APDU_duplicateDetectionResponse 25
	} u;
};

struct Z_InitRequest {
	Z_ReferenceId *referenceId; /* OPT */
	Z_ProtocolVersion *protocolVersion;
	Z_Options *options;
	Odr_int *preferredMessageSize;
	Odr_int *maximumRecordSize;
	Z_IdAuthentication *idAuthentication; /* OPT */
	Z_InternationalString *implementationId; /* OPT */
	Z_InternationalString *implementationName; /* OPT */
	Z_InternationalString *implementationVersion; /* OPT */
	Z_External *userInformationField; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_IdPass {
	Z_InternationalString *groupId; /* OPT */
	Z_InternationalString *userId; /* OPT */
	Z_InternationalString *password; /* OPT */
};

struct Z_IdAuthentication {
	int which;
	union {
		char *open;
		Z_IdPass *idPass;
		Odr_null *anonymous;
		Z_External *other;
#define Z_IdAuthentication_open 1
#define Z_IdAuthentication_idPass 2
#define Z_IdAuthentication_anonymous 3
#define Z_IdAuthentication_other 4
	} u;
};

struct Z_InitResponse {
	Z_ReferenceId *referenceId; /* OPT */
	Z_ProtocolVersion *protocolVersion;
	Z_Options *options;
	Odr_int *preferredMessageSize;
	Odr_int *maximumRecordSize;
	Odr_bool *result;
	Z_InternationalString *implementationId; /* OPT */
	Z_InternationalString *implementationName; /* OPT */
	Z_InternationalString *implementationVersion; /* OPT */
	Z_External *userInformationField; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

#define Z_ProtocolVersion_1 0
#define Z_ProtocolVersion_2 1
#define Z_ProtocolVersion_3 2

#define Z_Options_search 0
#define Z_Options_present 1
#define Z_Options_delSet 2
#define Z_Options_resourceReport 3
#define Z_Options_triggerResourceCtrl 4
#define Z_Options_resourceCtrl 5
#define Z_Options_accessCtrl 6
#define Z_Options_scan 7
#define Z_Options_sort 8
#define Z_Options_extendedServices 10
#define Z_Options_level_1Segmentation 11
#define Z_Options_level_2Segmentation 12
#define Z_Options_concurrentOperations 13
#define Z_Options_namedResultSets 14
#define Z_Options_encapsulation 15
#define Z_Options_resultCount 16
#define Z_Options_negotiationModel 17
#define Z_Options_duplicateDetection 18
#define Z_Options_queryType104 19
#define Z_Options_pQESCorrection 20
#define Z_Options_stringSchema 21

struct Z_SearchRequest {
	Z_ReferenceId *referenceId; /* OPT */
	Odr_int *smallSetUpperBound;
	Odr_int *largeSetLowerBound;
	Odr_int *mediumSetPresentNumber;
	Odr_bool *replaceIndicator;
	Z_InternationalString *resultSetName;
	int num_databaseNames;
	Z_DatabaseName **databaseNames;
	Z_ElementSetNames *smallSetElementSetNames; /* OPT */
	Z_ElementSetNames *mediumSetElementSetNames; /* OPT */
	Odr_oid *preferredRecordSyntax; /* OPT */
	Z_Query *query;
	Z_OtherInformation *additionalSearchInfo; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_Query {
	int which;
	union {
		void *type_0;
		Z_RPNQuery *type_1;
		Odr_oct *type_2;
		Odr_oct *type_100;
		Z_RPNQuery *type_101;
		Odr_oct *type_102;
		Z_External *type_104;
#define Z_Query_type_0 1
#define Z_Query_type_1 2
#define Z_Query_type_2 3
#define Z_Query_type_100 4
#define Z_Query_type_101 5
#define Z_Query_type_102 6
#define Z_Query_type_104 7
	} u;
};

struct Z_RPNQuery {
	Z_AttributeSetId *attributeSetId;
	Z_RPNStructure *RPNStructure;
};

struct Z_Complex {
	Z_RPNStructure *s1;
	Z_RPNStructure *s2;
	Z_Operator *roperator;
};

struct Z_RPNStructure {
	int which;
	union {
		Z_Operand *simple;
		Z_Complex *complex;
#define Z_RPNStructure_simple 1
#define Z_RPNStructure_complex 2
	} u;
};

struct Z_Operand {
	int which;
	union {
		Z_AttributesPlusTerm *attributesPlusTerm;
		Z_ResultSetId *resultSetId;
		Z_ResultSetPlusAttributes *resultAttr;
#define Z_Operand_APT 1
#define Z_Operand_resultSetId 2
#define Z_Operand_resultAttr 3
	} u;
};

struct Z_AttributesPlusTerm {
	Z_AttributeList *attributes;
	Z_Term *term;
};

struct Z_ResultSetPlusAttributes {
	Z_ResultSetId *resultSet;
	Z_AttributeList *attributes;
};

struct Z_AttributeList {
	int num_attributes;
	Z_AttributeElement **attributes;
};

struct Z_Term {
	int which;
	union {
		Odr_oct *general;
		Odr_int *numeric;
		Z_InternationalString *characterString;
		Odr_oid *oid;
		char *dateTime;
		Z_External *external;
		Z_IntUnit *integerAndUnit;
		Odr_null *null;
#define Z_Term_general 1
#define Z_Term_numeric 2
#define Z_Term_characterString 3
#define Z_Term_oid 4
#define Z_Term_dateTime 5
#define Z_Term_external 6
#define Z_Term_integerAndUnit 7
#define Z_Term_null 8
	} u;
};

struct Z_Operator {
	int which;
	union {
		Odr_null *op_and;
		Odr_null *op_or;
		Odr_null *and_not;
		Z_ProximityOperator *prox;
#define Z_Operator_and 1
#define Z_Operator_or 2
#define Z_Operator_and_not 3
#define Z_Operator_prox 4
	} u;
};

struct Z_ComplexAttribute {
	int num_list;
	Z_StringOrNumeric **list;
	int num_semanticAction;
	Odr_int **semanticAction; /* OPT */
};

struct Z_AttributeElement {
	Z_AttributeSetId *attributeSet; /* OPT */
	Odr_int *attributeType;
	int which;
	union {
		Odr_int *numeric;
		Z_ComplexAttribute *complex;
#define Z_AttributeValue_numeric 1
#define Z_AttributeValue_complex 2
	} value;
};

struct Z_ProximityOperator {
	Odr_bool *exclusion; /* OPT */
	Odr_int *distance;
	Odr_bool *ordered;
#define Z_ProximityOperator_Prox_lessThan 1
#define Z_ProximityOperator_Prox_lessThanOrEqual 2
#define Z_ProximityOperator_Prox_equal 3
#define Z_ProximityOperator_Prox_greaterThanOrEqual 4
#define Z_ProximityOperator_Prox_greaterThan 5
#define Z_ProximityOperator_Prox_notEqual 6
	Odr_int *relationType;
	int which;
	union {
		Z_ProxUnit *known;
		Odr_int *zprivate;
#define Z_ProximityOperator_known 1
#define Z_ProximityOperator_private 2
	} u;
};

#define Z_ProxUnit_character 1
#define Z_ProxUnit_word 2
#define Z_ProxUnit_sentence 3
#define Z_ProxUnit_paragraph 4
#define Z_ProxUnit_section 5
#define Z_ProxUnit_chapter 6
#define Z_ProxUnit_document 7
#define Z_ProxUnit_element 8
#define Z_ProxUnit_subelement 9
#define Z_ProxUnit_elementType 10
#define Z_ProxUnit_byte 11

struct Z_SearchResponse {
	Z_ReferenceId *referenceId; /* OPT */
	Odr_int *resultCount;
	Odr_int *numberOfRecordsReturned;
	Odr_int *nextResultSetPosition;
	Odr_bool *searchStatus;
#define Z_SearchResponse_subset 1
#define Z_SearchResponse_interim 2
#define Z_SearchResponse_none 3
#define Z_SearchResponse_estimate 4
	Odr_int *resultSetStatus; /* OPT */
	Z_PresentStatus *presentStatus; /* OPT */
	Z_Records *records; /* OPT */
	Z_OtherInformation *additionalSearchInfo; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_RecordComposition {
	int which;
	union {
		Z_ElementSetNames *simple;
		Z_CompSpec *complex;
#define Z_RecordComp_simple 1
#define Z_RecordComp_complex 2
	} u;
};

struct Z_PresentRequest {
	Z_ReferenceId *referenceId; /* OPT */
	Z_ResultSetId *resultSetId;
	Odr_int *resultSetStartPoint;
	Odr_int *numberOfRecordsRequested;
	int num_ranges;
	Z_Range **additionalRanges; /* OPT */
	Z_RecordComposition *recordComposition; /* OPT */
	Odr_oid *preferredRecordSyntax; /* OPT */
	Odr_int *maxSegmentCount; /* OPT */
	Odr_int *maxRecordSize; /* OPT */
	Odr_int *maxSegmentSize; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_Segment {
	Z_ReferenceId *referenceId; /* OPT */
	Odr_int *numberOfRecordsReturned;
	int num_segmentRecords;
	Z_NamePlusRecord **segmentRecords;
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_PresentResponse {
	Z_ReferenceId *referenceId; /* OPT */
	Odr_int *numberOfRecordsReturned;
	Odr_int *nextResultSetPosition;
	Z_PresentStatus *presentStatus;
	Z_Records *records; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_NamePlusRecordList {
	int num_records;
	Z_NamePlusRecord **records;
};

struct Z_DiagRecs {
	int num_diagRecs;
	Z_DiagRec **diagRecs;
};

struct Z_Records {
	int which;
	union {
		Z_NamePlusRecordList *databaseOrSurDiagnostics;
		Z_DefaultDiagFormat *nonSurrogateDiagnostic;
		Z_DiagRecs *multipleNonSurDiagnostics;
#define Z_Records_DBOSD 1
#define Z_Records_NSD 2
#define Z_Records_multipleNSD 3
	} u;
};

struct Z_NamePlusRecord {
	Z_DatabaseName *databaseName; /* OPT */
	int which;
	union {
		Z_External *databaseRecord;
		Z_DiagRec *surrogateDiagnostic;
		Z_FragmentSyntax *startingFragment;
		Z_FragmentSyntax *intermediateFragment;
		Z_FragmentSyntax *finalFragment;
#define Z_NamePlusRecord_databaseRecord 1
#define Z_NamePlusRecord_surrogateDiagnostic 2
#define Z_NamePlusRecord_startingFragment 3
#define Z_NamePlusRecord_intermediateFragment 4
#define Z_NamePlusRecord_finalFragment 5
	} u;
};

struct Z_FragmentSyntax {
	int which;
	union {
		Z_External *externallyTagged;
		Odr_oct *notExternallyTagged;
#define Z_FragmentSyntax_externallyTagged 1
#define Z_FragmentSyntax_notExternallyTagged 2
	} u;
};

struct Z_DiagRec {
	int which;
	union {
		Z_DefaultDiagFormat *defaultFormat;
		Z_External *externallyDefined;
#define Z_DiagRec_defaultFormat 1
#define Z_DiagRec_externallyDefined 2
	} u;
};

struct Z_DefaultDiagFormat {
	Odr_oid *diagnosticSetId;
	Odr_int *condition;
	int which;
	union {
		char *v2Addinfo;
		Z_InternationalString *v3Addinfo;
#define Z_DefaultDiagFormat_v2Addinfo 1
#define Z_DefaultDiagFormat_v3Addinfo 2
	} u;
};

struct Z_Range {
	Odr_int *startingPosition;
	Odr_int *numberOfRecords;
};

struct Z_DatabaseSpecificUnit {
	Z_DatabaseName *dbName;
	Z_ElementSetName *esn;
};

struct Z_DatabaseSpecific {
	int num;
	Z_DatabaseSpecificUnit **elements;
};

struct Z_ElementSetNames {
	int which;
	union {
		Z_InternationalString *generic;
		Z_DatabaseSpecific *databaseSpecific;
#define Z_ElementSetNames_generic 1
#define Z_ElementSetNames_databaseSpecific 2
	} u;
};

#define Z_PresentStatus_success 0
#define Z_PresentStatus_partial_1 1
#define Z_PresentStatus_partial_2 2
#define Z_PresentStatus_partial_3 3
#define Z_PresentStatus_partial_4 4
#define Z_PresentStatus_failure 5

struct Z_DbSpecific {
	Z_DatabaseName *db;
	Z_Specification *spec;
};

struct Z_CompSpec {
	Odr_bool *selectAlternativeSyntax;
	Z_Specification *generic; /* OPT */
	int num_dbSpecific;
	Z_DbSpecific **dbSpecific; /* OPT */
	int num_recordSyntax;
	Odr_oid **recordSyntax; /* OPT */
};

struct Z_ElementSpec {
	int which;
	union {
		Z_InternationalString *elementSetName;
		Z_External *externalSpec;
#define Z_ElementSpec_elementSetName 1
#define Z_ElementSpec_externalSpec 2
	} u;
};

struct Z_Specification {
	int which;
	union {
		Odr_oid *oid;
		Z_InternationalString *uri;
#define Z_Schema_oid 1
#define Z_Schema_uri 2
	} schema; /* OPT */
	Z_ElementSpec *elementSpec; /* OPT */
};

struct Z_DeleteResultSetRequest {
	Z_ReferenceId *referenceId; /* OPT */
#define Z_DeleteResultSetRequest_list 0
#define Z_DeleteResultSetRequest_all 1
	Odr_int *deleteFunction;
	int num_resultSetList;
	Z_ResultSetId **resultSetList; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_DeleteResultSetResponse {
	Z_ReferenceId *referenceId; /* OPT */
	Z_DeleteStatus *deleteOperationStatus;
	Z_ListStatuses *deleteListStatuses; /* OPT */
	Odr_int *numberNotDeleted; /* OPT */
	Z_ListStatuses *bulkStatuses; /* OPT */
	Z_InternationalString *deleteMessage; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_ListStatus {
	Z_ResultSetId *id;
	Z_DeleteStatus *status;
};

struct Z_ListStatuses {
	int num;
	Z_ListStatus **elements;
};

#define Z_DeleteStatus_success 0
#define Z_DeleteStatus_resultSetDidNotExist 1
#define Z_DeleteStatus_previouslyDeletedByTarget 2
#define Z_DeleteStatus_systemProblemAtTarget 3
#define Z_DeleteStatus_accessNotAllowed 4
#define Z_DeleteStatus_resourceControlAtOrigin 5
#define Z_DeleteStatus_resourceControlAtTarget 6
#define Z_DeleteStatus_bulkDeleteNotSupported 7
#define Z_DeleteStatus_notAllRsltSetsDeletedOnBulkDlte 8
#define Z_DeleteStatus_notAllRequestedResultSetsDeleted 9
#define Z_DeleteStatus_resultSetInUse 10

struct Z_AccessControlRequest {
	Z_ReferenceId *referenceId; /* OPT */
	int which;
	union {
		Odr_oct *simpleForm;
		Z_External *externallyDefined;
#define Z_AccessControlRequest_simpleForm 1
#define Z_AccessControlRequest_externallyDefined 2
	} u;
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_AccessControlResponse {
	Z_ReferenceId *referenceId; /* OPT */
	int which;
	union {
		Odr_oct *simpleForm;
		Z_External *externallyDefined;
#define Z_AccessControlResponse_simpleForm 1
#define Z_AccessControlResponse_externallyDefined 2
	} u; /* OPT */
	Z_DiagRec *diagnostic; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_ResourceControlRequest {
	Z_ReferenceId *referenceId; /* OPT */
	Odr_bool *suspendedFlag; /* OPT */
	Z_ResourceReport *resourceReport; /* OPT */
#define Z_ResourceControlRequest_subset 1
#define Z_ResourceControlRequest_interim 2
#define Z_ResourceControlRequest_none 3
	Odr_int *partialResultsAvailable; /* OPT */
	Odr_bool *responseRequired;
	Odr_bool *triggeredRequestFlag; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_ResourceControlResponse {
	Z_ReferenceId *referenceId; /* OPT */
	Odr_bool *continueFlag;
	Odr_bool *resultSetWanted; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_TriggerResourceControlRequest {
	Z_ReferenceId *referenceId; /* OPT */
#define Z_TriggerResourceControlRequest_resourceReport 1
#define Z_TriggerResourceControlRequest_resourceControl 2
#define Z_TriggerResourceControlRequest_cancel 3
	Odr_int *requestedAction;
	Z_ResourceReportId *prefResourceReportFormat; /* OPT */
	Odr_bool *resultSetWanted; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_ResourceReportRequest {
	Z_ReferenceId *referenceId; /* OPT */
	Z_ReferenceId *opId; /* OPT */
	Z_ResourceReportId *prefResourceReportFormat; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_ResourceReportResponse {
	Z_ReferenceId *referenceId; /* OPT */
#define Z_ResourceReportResponse_success 0
#define Z_ResourceReportResponse_partial 1
#define Z_ResourceReportResponse_failure_1 2
#define Z_ResourceReportResponse_failure_2 3
#define Z_ResourceReportResponse_failure_3 4
#define Z_ResourceReportResponse_failure_4 5
#define Z_ResourceReportResponse_failure_5 6
#define Z_ResourceReportResponse_failure_6 7
	Odr_int *resourceReportStatus;
	Z_ResourceReport *resourceReport; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};





struct Z_ScanRequest {
	Z_ReferenceId *referenceId; /* OPT */
	int num_databaseNames;
	Z_DatabaseName **databaseNames;
	Z_AttributeSetId *attributeSet; /* OPT */
	Z_AttributesPlusTerm *termListAndStartPoint;
	Odr_int *stepSize; /* OPT */
	Odr_int *numberOfTermsRequested;
	Odr_int *preferredPositionInResponse; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_ScanResponse {
	Z_ReferenceId *referenceId; /* OPT */
	Odr_int *stepSize; /* OPT */
#define Z_Scan_success 0
#define Z_Scan_partial_1 1
#define Z_Scan_partial_2 2
#define Z_Scan_partial_3 3
#define Z_Scan_partial_4 4
#define Z_Scan_partial_5 5
#define Z_Scan_failure 6
	Odr_int *scanStatus;
	Odr_int *numberOfEntriesReturned;
	Odr_int *positionOfTerm; /* OPT */
	Z_ListEntries *entries; /* OPT */
	Z_AttributeSetId *attributeSet; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_ListEntries {
	int num_entries;
	Z_Entry **entries; /* OPT */
	int num_nonsurrogateDiagnostics;
	Z_DiagRec **nonsurrogateDiagnostics; /* OPT */
};

struct Z_Entry {
	int which;
	union {
		Z_TermInfo *termInfo;
		Z_DiagRec *surrogateDiagnostic;
#define Z_Entry_termInfo 1
#define Z_Entry_surrogateDiagnostic 2
	} u;
};

struct Z_TermInfo {
	Z_Term *term;
	Z_InternationalString *displayTerm; /* OPT */
	Z_AttributeList *suggestedAttributes; /* OPT */
	int num_alternativeTerm;
	Z_AttributesPlusTerm **alternativeTerm; /* OPT */
	Odr_int *globalOccurrences; /* OPT */
	Z_OccurrenceByAttributes *byAttributes; /* OPT */
	Z_OtherInformation *otherTermInfo; /* OPT */
};

struct Z_byDatabaseList_s {
	Z_DatabaseName *db;
	Odr_int *num; /* OPT */
	Z_OtherInformation *otherDbInfo; /* OPT */
};

struct Z_byDatabaseList {
	int num;
	Z_byDatabaseList_s **elements;
};

struct Z_OccurrenceByAttributesElem {
	Z_AttributeList *attributes;
	int which;
	union {
		Odr_int *global;
		Z_byDatabaseList *byDatabase;
#define Z_OccurrenceByAttributesElem_global 1
#define Z_OccurrenceByAttributesElem_byDatabase 2
	} u; /* OPT */
	Z_OtherInformation *otherOccurInfo; /* OPT */
};

struct Z_OccurrenceByAttributes {
	int num;
	Z_OccurrenceByAttributesElem **elements;
};

struct Z_SortKeySpecList {
	int num_specs;
	Z_SortKeySpec **specs;
};

struct Z_SortRequest {
	Z_ReferenceId *referenceId; /* OPT */
	int num_inputResultSetNames;
	Z_InternationalString **inputResultSetNames;
	Z_InternationalString *sortedResultSetName;
	Z_SortKeySpecList *sortSequence;
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_SortResponse {
	Z_ReferenceId *referenceId; /* OPT */
#define Z_SortResponse_success 0
#define Z_SortResponse_partial_1 1
#define Z_SortResponse_failure 2
	Odr_int *sortStatus;
#define Z_SortResponse_empty 1
#define Z_SortResponse_interim 2
#define Z_SortResponse_unchanged 3
#define Z_SortResponse_none 4
	Odr_int *resultSetStatus; /* OPT */
	int num_diagnostics;
	Z_DiagRec **diagnostics; /* OPT */
	Odr_int *resultCount; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_SortKeySpec {
	Z_SortElement *sortElement;
#define Z_SortKeySpec_ascending 0
#define Z_SortKeySpec_descending 1
#define Z_SortKeySpec_ascendingByFrequency 3
#define Z_SortKeySpec_descendingByfrequency 4
	Odr_int *sortRelation;
#define Z_SortKeySpec_caseSensitive 0
#define Z_SortKeySpec_caseInsensitive 1
	Odr_int *caseSensitivity;
	int which;
	union {
		Odr_null *abort;
		Odr_null *null;
		Odr_oct *missingValueData;
#define Z_SortKeySpec_abort 1
#define Z_SortKeySpec_null 2
#define Z_SortKeySpec_missingValueData 3
	} u; /* OPT */
};

struct Z_SortDbSpecificList_s {
	Z_DatabaseName *databaseName;
	Z_SortKey *dbSort;
};

struct Z_SortDbSpecificList {
	int num;
	Z_SortDbSpecificList_s **elements;
};

struct Z_SortElement {
	int which;
	union {
		Z_SortKey *generic;
		Z_SortDbSpecificList *databaseSpecific;
#define Z_SortElement_generic 1
#define Z_SortElement_databaseSpecific 2
	} u;
};

struct Z_SortAttributes {
	Z_AttributeSetId *id;
	Z_AttributeList *list;
};

struct Z_SortKey {
	int which;
	union {
		Z_InternationalString *sortField;
		Z_Specification *elementSpec;
		Z_SortAttributes *sortAttributes;
#define Z_SortKey_sortField 1
#define Z_SortKey_elementSpec 2
#define Z_SortKey_sortAttributes 3
	} u;
};

struct Z_ExtendedServicesRequest {
	Z_ReferenceId *referenceId; /* OPT */
#define Z_ExtendedServicesRequest_create 1
#define Z_ExtendedServicesRequest_delete 2
#define Z_ExtendedServicesRequest_modify 3
	Odr_int *function;
	Odr_oid *packageType;
	Z_InternationalString *packageName; /* OPT */
	Z_InternationalString *userId; /* OPT */
	Z_IntUnit *retentionTime; /* OPT */
	Z_Permissions *permissions; /* OPT */
	Z_InternationalString *description; /* OPT */
	Z_External *taskSpecificParameters; /* OPT */
#define Z_ExtendedServicesRequest_wait 1
#define Z_ExtendedServicesRequest_waitIfPossible 2
#define Z_ExtendedServicesRequest_dontWait 3
#define Z_ExtendedServicesRequest_dontReturnPackage 4
	Odr_int *waitAction;
	Z_ElementSetName *elements; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_ExtendedServicesResponse {
	Z_ReferenceId *referenceId; /* OPT */
#define Z_ExtendedServicesResponse_done 1
#define Z_ExtendedServicesResponse_accepted 2
#define Z_ExtendedServicesResponse_failure 3
	Odr_int *operationStatus;
	int num_diagnostics;
	Z_DiagRec **diagnostics; /* OPT */
	Z_External *taskPackage; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_Permissions_s {
	Z_InternationalString *userId; /* OPT */
#define Z_Permissions_s_delete 1
#define Z_Permissions_s_modifyContents 2
#define Z_Permissions_s_modifyPermissions 3
#define Z_Permissions_s_present 4
#define Z_Permissions_s_invoke 5
	int num_allowableFunctions;
	Odr_int **allowableFunctions;
};

struct Z_Permissions {
	int num;
	Z_Permissions_s **elements;
};

struct Z_Close {
	Z_ReferenceId *referenceId; /* OPT */
	Z_CloseReason *closeReason;
	Z_InternationalString *diagnosticInformation; /* OPT */
	Z_ResourceReportId *resourceReportFormat; /* OPT */
	Z_ResourceReport *resourceReport; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

#define Z_Close_finished 0
#define Z_Close_shutdown 1
#define Z_Close_systemProblem 2
#define Z_Close_costLimit 3
#define Z_Close_resources 4
#define Z_Close_securityViolation 5
#define Z_Close_protocolError 6
#define Z_Close_lackOfActivity 7
#define Z_Close_peerAbort 8
#define Z_Close_unspecified 9

struct Z_DuplicateDetectionRequest {
	Z_ReferenceId *referenceId; /* OPT */
	int num_inputResultSetIds;
	Z_InternationalString **inputResultSetIds;
	Z_InternationalString *outputResultSetName;
	Z_External *applicablePortionOfRecord; /* OPT */
	int num_duplicateDetectionCriteria;
	Z_DuplicateDetectionCriterion **duplicateDetectionCriteria; /* OPT */
	Odr_bool *clustering; /* OPT */
	int num_retentionCriteria;
	Z_RetentionCriterion **retentionCriteria;
	int num_sortCriteria;
	Z_SortCriterion **sortCriteria; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_DuplicateDetectionCriterion {
	int which;
	union {
		Odr_int *levelOfMatch;
		Odr_null *caseSensitive;
		Odr_null *punctuationSensitive;
		Z_External *regularExpression;
		Odr_null *rsDuplicates;
#define Z_DuplicateDetectionCriterion_levelOfMatch 1
#define Z_DuplicateDetectionCriterion_caseSensitive 2
#define Z_DuplicateDetectionCriterion_punctuationSensitive 3
#define Z_DuplicateDetectionCriterion_regularExpression 4
#define Z_DuplicateDetectionCriterion_rsDuplicates 5
	} u;
};

struct Z_RetentionCriterion {
	int which;
	union {
		Odr_int *numberOfEntries;
		Odr_int *percentOfEntries;
		Odr_null *duplicatesOnly;
		Odr_null *discardRsDuplicates;
#define Z_RetentionCriterion_numberOfEntries 1
#define Z_RetentionCriterion_percentOfEntries 2
#define Z_RetentionCriterion_duplicatesOnly 3
#define Z_RetentionCriterion_discardRsDuplicates 4
	} u;
};

struct Z_SortCriterionPreferredDatabases {
	int num;
	Z_InternationalString **elements;
};

struct Z_SortCriterion {
	int which;
	union {
		Odr_null *mostComprehensive;
		Odr_null *leastConmprehensive;
		Odr_null *mostRecent;
		Odr_null *oldest;
		Odr_null *leastCost;
		Z_SortCriterionPreferredDatabases *preferredDatabases;
#define Z_SortCriterion_mostComprehensive 1
#define Z_SortCriterion_leastConmprehensive 2
#define Z_SortCriterion_mostRecent 3
#define Z_SortCriterion_oldest 4
#define Z_SortCriterion_leastCost 5
#define Z_SortCriterion_preferredDatabases 6
	} u;
};

struct Z_DuplicateDetectionResponse {
	Z_ReferenceId *referenceId; /* OPT */
#define Z_DuplicateDetectionResponse_success 0
#define Z_DuplicateDetectionResponse_failure 1
	Odr_int *status;
	Odr_int *resultSetCount; /* OPT */
	int num_diagnostics;
	Z_DiagRec **diagnostics; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};











struct Z_OtherInformationUnit {
	Z_InfoCategory *category; /* OPT */
	int which;
	union {
		Z_InternationalString *characterInfo;
		Odr_oct *binaryInfo;
		Z_External *externallyDefinedInfo;
		Odr_oid *oid;
#define Z_OtherInfo_characterInfo 1
#define Z_OtherInfo_binaryInfo 2
#define Z_OtherInfo_externallyDefinedInfo 3
#define Z_OtherInfo_oid 4
	} information;
};

struct Z_OtherInformation {
	int num_elements;
	Z_OtherInformationUnit **list;
};

struct Z_InfoCategory {
	Odr_oid *categoryTypeId; /* OPT */
	Odr_int *categoryValue;
};

struct Z_IntUnit {
	Odr_int *value;
	Z_Unit *unitUsed;
};

struct Z_Unit {
	Z_InternationalString *unitSystem; /* OPT */
	Z_StringOrNumeric *unitType; /* OPT */
	Z_StringOrNumeric *unit; /* OPT */
	Odr_int *scaleFactor; /* OPT */
};



struct Z_StringOrNumeric {
	int which;
	union {
		Z_InternationalString *string;
		Odr_int *numeric;
#define Z_StringOrNumeric_string 1
#define Z_StringOrNumeric_numeric 2
	} u;
};

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

int z_ANY_type_0 (ODR o, void **p, int opt);

#ifdef __cplusplus
}
#endif

#endif
