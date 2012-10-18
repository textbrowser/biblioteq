/** \file z-exp.h
    \brief ASN.1 Module RecordSyntax-explain

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_exp_H
#define z_exp_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_ExplainRecord Z_ExplainRecord;
YAZ_EXPORT int z_ExplainRecord(ODR o, Z_ExplainRecord **p, int opt, const char *name);

typedef struct Z_TargetInfo Z_TargetInfo;
YAZ_EXPORT int z_TargetInfo(ODR o, Z_TargetInfo **p, int opt, const char *name);

typedef struct Z_DatabaseInfo Z_DatabaseInfo;
YAZ_EXPORT int z_DatabaseInfo(ODR o, Z_DatabaseInfo **p, int opt, const char *name);

typedef struct Z_TagTypeMapping Z_TagTypeMapping;
YAZ_EXPORT int z_TagTypeMapping(ODR o, Z_TagTypeMapping **p, int opt, const char *name);

typedef struct Z_SchemaInfo Z_SchemaInfo;
YAZ_EXPORT int z_SchemaInfo(ODR o, Z_SchemaInfo **p, int opt, const char *name);

typedef struct Z_ElementInfo Z_ElementInfo;
YAZ_EXPORT int z_ElementInfo(ODR o, Z_ElementInfo **p, int opt, const char *name);

typedef struct Z_PathUnit Z_PathUnit;
YAZ_EXPORT int z_PathUnit(ODR o, Z_PathUnit **p, int opt, const char *name);

typedef struct Z_Path Z_Path;
YAZ_EXPORT int z_Path(ODR o, Z_Path **p, int opt, const char *name);

typedef struct Z_ElementInfoList Z_ElementInfoList;
YAZ_EXPORT int z_ElementInfoList(ODR o, Z_ElementInfoList **p, int opt, const char *name);

typedef struct Z_ElementDataType Z_ElementDataType;
YAZ_EXPORT int z_ElementDataType(ODR o, Z_ElementDataType **p, int opt, const char *name);

typedef Odr_int  Z_PrimitiveDataType;
YAZ_EXPORT int z_PrimitiveDataType(ODR o, Z_PrimitiveDataType **p, int opt, const char *name);

typedef struct Z_TagSetElements Z_TagSetElements;
YAZ_EXPORT int z_TagSetElements(ODR o, Z_TagSetElements **p, int opt, const char *name);

typedef struct Z_TagSetInfo Z_TagSetInfo;
YAZ_EXPORT int z_TagSetInfo(ODR o, Z_TagSetInfo **p, int opt, const char *name);

typedef struct Z_RecordSyntaxInfo Z_RecordSyntaxInfo;
YAZ_EXPORT int z_RecordSyntaxInfo(ODR o, Z_RecordSyntaxInfo **p, int opt, const char *name);

typedef struct Z_AttributeSetInfo Z_AttributeSetInfo;
YAZ_EXPORT int z_AttributeSetInfo(ODR o, Z_AttributeSetInfo **p, int opt, const char *name);

typedef struct Z_AttributeType Z_AttributeType;
YAZ_EXPORT int z_AttributeType(ODR o, Z_AttributeType **p, int opt, const char *name);

typedef struct Z_AttributeDescription Z_AttributeDescription;
YAZ_EXPORT int z_AttributeDescription(ODR o, Z_AttributeDescription **p, int opt, const char *name);

typedef struct Z_TermListElement Z_TermListElement;
YAZ_EXPORT int z_TermListElement(ODR o, Z_TermListElement **p, int opt, const char *name);

typedef struct Z_TermListInfo Z_TermListInfo;
YAZ_EXPORT int z_TermListInfo(ODR o, Z_TermListInfo **p, int opt, const char *name);

typedef struct Z_ExtendedServicesInfo Z_ExtendedServicesInfo;
YAZ_EXPORT int z_ExtendedServicesInfo(ODR o, Z_ExtendedServicesInfo **p, int opt, const char *name);

typedef struct Z_AttributeDetails Z_AttributeDetails;
YAZ_EXPORT int z_AttributeDetails(ODR o, Z_AttributeDetails **p, int opt, const char *name);

typedef struct Z_AttributeSetDetails Z_AttributeSetDetails;
YAZ_EXPORT int z_AttributeSetDetails(ODR o, Z_AttributeSetDetails **p, int opt, const char *name);

typedef struct Z_AttributeTypeDetails Z_AttributeTypeDetails;
YAZ_EXPORT int z_AttributeTypeDetails(ODR o, Z_AttributeTypeDetails **p, int opt, const char *name);

typedef struct Z_OmittedAttributeInterpretation Z_OmittedAttributeInterpretation;
YAZ_EXPORT int z_OmittedAttributeInterpretation(ODR o, Z_OmittedAttributeInterpretation **p, int opt, const char *name);

typedef struct Z_AttributeValue Z_AttributeValue;
YAZ_EXPORT int z_AttributeValue(ODR o, Z_AttributeValue **p, int opt, const char *name);

typedef struct Z_EScanInfo Z_EScanInfo;
YAZ_EXPORT int z_EScanInfo(ODR o, Z_EScanInfo **p, int opt, const char *name);

typedef struct Z_TermListDetails Z_TermListDetails;
YAZ_EXPORT int z_TermListDetails(ODR o, Z_TermListDetails **p, int opt, const char *name);

typedef struct Z_ElementSetDetails Z_ElementSetDetails;
YAZ_EXPORT int z_ElementSetDetails(ODR o, Z_ElementSetDetails **p, int opt, const char *name);

typedef struct Z_RetrievalRecordDetails Z_RetrievalRecordDetails;
YAZ_EXPORT int z_RetrievalRecordDetails(ODR o, Z_RetrievalRecordDetails **p, int opt, const char *name);

typedef struct Z_PerElementDetails Z_PerElementDetails;
YAZ_EXPORT int z_PerElementDetails(ODR o, Z_PerElementDetails **p, int opt, const char *name);

typedef struct Z_RecordTag Z_RecordTag;
YAZ_EXPORT int z_RecordTag(ODR o, Z_RecordTag **p, int opt, const char *name);

typedef struct Z_SortDetails Z_SortDetails;
YAZ_EXPORT int z_SortDetails(ODR o, Z_SortDetails **p, int opt, const char *name);

typedef struct Z_SortKeyDetails Z_SortKeyDetails;
YAZ_EXPORT int z_SortKeyDetails(ODR o, Z_SortKeyDetails **p, int opt, const char *name);

typedef struct Z_ProcessingInformation Z_ProcessingInformation;
YAZ_EXPORT int z_ProcessingInformation(ODR o, Z_ProcessingInformation **p, int opt, const char *name);

typedef struct Z_VariantSetInfo Z_VariantSetInfo;
YAZ_EXPORT int z_VariantSetInfo(ODR o, Z_VariantSetInfo **p, int opt, const char *name);

typedef struct Z_VariantClass Z_VariantClass;
YAZ_EXPORT int z_VariantClass(ODR o, Z_VariantClass **p, int opt, const char *name);

typedef struct Z_VariantType Z_VariantType;
YAZ_EXPORT int z_VariantType(ODR o, Z_VariantType **p, int opt, const char *name);

typedef struct Z_VariantValue Z_VariantValue;
YAZ_EXPORT int z_VariantValue(ODR o, Z_VariantValue **p, int opt, const char *name);

typedef struct Z_ValueSetEnumerated Z_ValueSetEnumerated;
YAZ_EXPORT int z_ValueSetEnumerated(ODR o, Z_ValueSetEnumerated **p, int opt, const char *name);

typedef struct Z_ValueSet Z_ValueSet;
YAZ_EXPORT int z_ValueSet(ODR o, Z_ValueSet **p, int opt, const char *name);

typedef struct Z_ValueRange Z_ValueRange;
YAZ_EXPORT int z_ValueRange(ODR o, Z_ValueRange **p, int opt, const char *name);

typedef struct Z_ValueDescription Z_ValueDescription;
YAZ_EXPORT int z_ValueDescription(ODR o, Z_ValueDescription **p, int opt, const char *name);

typedef struct Z_UnitInfo Z_UnitInfo;
YAZ_EXPORT int z_UnitInfo(ODR o, Z_UnitInfo **p, int opt, const char *name);

typedef struct Z_UnitType Z_UnitType;
YAZ_EXPORT int z_UnitType(ODR o, Z_UnitType **p, int opt, const char *name);

typedef struct Z_Units Z_Units;
YAZ_EXPORT int z_Units(ODR o, Z_Units **p, int opt, const char *name);

typedef struct Z_CategoryList Z_CategoryList;
YAZ_EXPORT int z_CategoryList(ODR o, Z_CategoryList **p, int opt, const char *name);

typedef struct Z_CategoryInfo Z_CategoryInfo;
YAZ_EXPORT int z_CategoryInfo(ODR o, Z_CategoryInfo **p, int opt, const char *name);

typedef struct Z_CommonInfo Z_CommonInfo;
YAZ_EXPORT int z_CommonInfo(ODR o, Z_CommonInfo **p, int opt, const char *name);

typedef struct Z_HumanStringUnit Z_HumanStringUnit;
YAZ_EXPORT int z_HumanStringUnit(ODR o, Z_HumanStringUnit **p, int opt, const char *name);

typedef struct Z_HumanString Z_HumanString;
YAZ_EXPORT int z_HumanString(ODR o, Z_HumanString **p, int opt, const char *name);

typedef struct Z_IconObjectUnit Z_IconObjectUnit;
YAZ_EXPORT int z_IconObjectUnit(ODR o, Z_IconObjectUnit **p, int opt, const char *name);

typedef struct Z_IconObject Z_IconObject;
YAZ_EXPORT int z_IconObject(ODR o, Z_IconObject **p, int opt, const char *name);

typedef Z_InternationalString  Z_LanguageCode;
YAZ_EXPORT int z_LanguageCode(ODR o, Z_LanguageCode **p, int opt, const char *name);

typedef struct Z_ContactInfo Z_ContactInfo;
YAZ_EXPORT int z_ContactInfo(ODR o, Z_ContactInfo **p, int opt, const char *name);

typedef struct Z_NetworkAddressIA Z_NetworkAddressIA;
YAZ_EXPORT int z_NetworkAddressIA(ODR o, Z_NetworkAddressIA **p, int opt, const char *name);

typedef struct Z_NetworkAddressOPA Z_NetworkAddressOPA;
YAZ_EXPORT int z_NetworkAddressOPA(ODR o, Z_NetworkAddressOPA **p, int opt, const char *name);

typedef struct Z_NetworkAddressOther Z_NetworkAddressOther;
YAZ_EXPORT int z_NetworkAddressOther(ODR o, Z_NetworkAddressOther **p, int opt, const char *name);

typedef struct Z_NetworkAddress Z_NetworkAddress;
YAZ_EXPORT int z_NetworkAddress(ODR o, Z_NetworkAddress **p, int opt, const char *name);

typedef struct Z_AccessInfo Z_AccessInfo;
YAZ_EXPORT int z_AccessInfo(ODR o, Z_AccessInfo **p, int opt, const char *name);

typedef struct Z_QueryTypeDetails Z_QueryTypeDetails;
YAZ_EXPORT int z_QueryTypeDetails(ODR o, Z_QueryTypeDetails **p, int opt, const char *name);

typedef struct Z_PrivateCapOperator Z_PrivateCapOperator;
YAZ_EXPORT int z_PrivateCapOperator(ODR o, Z_PrivateCapOperator **p, int opt, const char *name);

typedef struct Z_PrivateCapabilities Z_PrivateCapabilities;
YAZ_EXPORT int z_PrivateCapabilities(ODR o, Z_PrivateCapabilities **p, int opt, const char *name);

typedef struct Z_RpnCapabilities Z_RpnCapabilities;
YAZ_EXPORT int z_RpnCapabilities(ODR o, Z_RpnCapabilities **p, int opt, const char *name);

typedef struct Z_Iso8777Capabilities Z_Iso8777Capabilities;
YAZ_EXPORT int z_Iso8777Capabilities(ODR o, Z_Iso8777Capabilities **p, int opt, const char *name);

typedef struct Z_ProxSupportPrivate Z_ProxSupportPrivate;
YAZ_EXPORT int z_ProxSupportPrivate(ODR o, Z_ProxSupportPrivate **p, int opt, const char *name);

typedef struct Z_ProxSupportUnit Z_ProxSupportUnit;
YAZ_EXPORT int z_ProxSupportUnit(ODR o, Z_ProxSupportUnit **p, int opt, const char *name);

typedef struct Z_ProximitySupport Z_ProximitySupport;
YAZ_EXPORT int z_ProximitySupport(ODR o, Z_ProximitySupport **p, int opt, const char *name);

typedef struct Z_SearchKey Z_SearchKey;
YAZ_EXPORT int z_SearchKey(ODR o, Z_SearchKey **p, int opt, const char *name);

typedef struct Z_AccessRestrictionsUnit Z_AccessRestrictionsUnit;
YAZ_EXPORT int z_AccessRestrictionsUnit(ODR o, Z_AccessRestrictionsUnit **p, int opt, const char *name);

typedef struct Z_AccessRestrictions Z_AccessRestrictions;
YAZ_EXPORT int z_AccessRestrictions(ODR o, Z_AccessRestrictions **p, int opt, const char *name);

typedef struct Z_CostsOtherCharge Z_CostsOtherCharge;
YAZ_EXPORT int z_CostsOtherCharge(ODR o, Z_CostsOtherCharge **p, int opt, const char *name);

typedef struct Z_Costs Z_Costs;
YAZ_EXPORT int z_Costs(ODR o, Z_Costs **p, int opt, const char *name);

typedef struct Z_Charge Z_Charge;
YAZ_EXPORT int z_Charge(ODR o, Z_Charge **p, int opt, const char *name);

typedef struct Z_DatabaseList Z_DatabaseList;
YAZ_EXPORT int z_DatabaseList(ODR o, Z_DatabaseList **p, int opt, const char *name);

typedef struct Z_AttributeCombinations Z_AttributeCombinations;
YAZ_EXPORT int z_AttributeCombinations(ODR o, Z_AttributeCombinations **p, int opt, const char *name);

typedef struct Z_AttributeCombination Z_AttributeCombination;
YAZ_EXPORT int z_AttributeCombination(ODR o, Z_AttributeCombination **p, int opt, const char *name);

typedef struct Z_AttributeValueList Z_AttributeValueList;
YAZ_EXPORT int z_AttributeValueList(ODR o, Z_AttributeValueList **p, int opt, const char *name);

typedef struct Z_AttributeOccurrence Z_AttributeOccurrence;
YAZ_EXPORT int z_AttributeOccurrence(ODR o, Z_AttributeOccurrence **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_ExplainRecord {
	int which;
	union {
		Z_TargetInfo *targetInfo;
		Z_DatabaseInfo *databaseInfo;
		Z_SchemaInfo *schemaInfo;
		Z_TagSetInfo *tagSetInfo;
		Z_RecordSyntaxInfo *recordSyntaxInfo;
		Z_AttributeSetInfo *attributeSetInfo;
		Z_TermListInfo *termListInfo;
		Z_ExtendedServicesInfo *extendedServicesInfo;
		Z_AttributeDetails *attributeDetails;
		Z_TermListDetails *termListDetails;
		Z_ElementSetDetails *elementSetDetails;
		Z_RetrievalRecordDetails *retrievalRecordDetails;
		Z_SortDetails *sortDetails;
		Z_ProcessingInformation *processing;
		Z_VariantSetInfo *variants;
		Z_UnitInfo *units;
		Z_CategoryList *categoryList;
#define Z_Explain_targetInfo 1
#define Z_Explain_databaseInfo 2
#define Z_Explain_schemaInfo 3
#define Z_Explain_tagSetInfo 4
#define Z_Explain_recordSyntaxInfo 5
#define Z_Explain_attributeSetInfo 6
#define Z_Explain_termListInfo 7
#define Z_Explain_extendedServicesInfo 8
#define Z_Explain_attributeDetails 9
#define Z_Explain_termListDetails 10
#define Z_Explain_elementSetDetails 11
#define Z_Explain_retrievalRecordDetails 12
#define Z_Explain_sortDetails 13
#define Z_Explain_processing 14
#define Z_Explain_variants 15
#define Z_Explain_units 16
#define Z_Explain_categoryList 17
	} u;
};

struct Z_TargetInfo {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_InternationalString *name;
	Z_HumanString *recentNews; /* OPT */
	Z_IconObject *icon; /* OPT */
	Odr_bool *namedResultSets;
	Odr_bool *multipleDBsearch;
	Odr_int *maxResultSets; /* OPT */
	Odr_int *maxResultSize; /* OPT */
	Odr_int *maxTerms; /* OPT */
	Z_IntUnit *timeoutInterval; /* OPT */
	Z_HumanString *welcomeMessage; /* OPT */
	Z_ContactInfo *contactInfo; /* OPT */
	Z_HumanString *description; /* OPT */
	int num_nicknames;
	Z_InternationalString **nicknames; /* OPT */
	Z_HumanString *usageRest; /* OPT */
	Z_HumanString *paymentAddr; /* OPT */
	Z_HumanString *hours; /* OPT */
	int num_dbCombinations;
	Z_DatabaseList **dbCombinations; /* OPT */
	int num_addresses;
	Z_NetworkAddress **addresses; /* OPT */
	int num_languages;
	Z_InternationalString **languages; /* OPT */
	Z_AccessInfo *commonAccessInfo; /* OPT */
};

struct Z_DatabaseInfo {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_DatabaseName *name;
	Odr_null *explainDatabase; /* OPT */
	int num_nicknames;
	Z_DatabaseName **nicknames; /* OPT */
	Z_IconObject *icon; /* OPT */
	Odr_bool *userFee;
	Odr_bool *available;
	Z_HumanString *titleString; /* OPT */
	int num_keywords;
	Z_HumanString **keywords; /* OPT */
	Z_HumanString *description; /* OPT */
	Z_DatabaseList *associatedDbs; /* OPT */
	Z_DatabaseList *subDbs; /* OPT */
	Z_HumanString *disclaimers; /* OPT */
	Z_HumanString *news; /* OPT */
	int which;
	union {
		Odr_int *actualNumber;
		Odr_int *approxNumber;
#define Z_DatabaseInfo_actualNumber 1
#define Z_DatabaseInfo_approxNumber 2
	} u; /* OPT */
	Z_HumanString *defaultOrder; /* OPT */
	Odr_int *avRecordSize; /* OPT */
	Odr_int *maxRecordSize; /* OPT */
	Z_HumanString *hours; /* OPT */
	Z_HumanString *bestTime; /* OPT */
	char *lastUpdate; /* OPT */
	Z_IntUnit *updateInterval; /* OPT */
	Z_HumanString *coverage; /* OPT */
	Odr_bool *proprietary; /* OPT */
	Z_HumanString *copyrightText; /* OPT */
	Z_HumanString *copyrightNotice; /* OPT */
	Z_ContactInfo *producerContactInfo; /* OPT */
	Z_ContactInfo *supplierContactInfo; /* OPT */
	Z_ContactInfo *submissionContactInfo; /* OPT */
	Z_AccessInfo *accessInfo; /* OPT */
};

struct Z_TagTypeMapping {
	Odr_int *tagType;
	Odr_oid *tagSet; /* OPT */
	Odr_null *defaultTagType; /* OPT */
};

struct Z_SchemaInfo {
	Z_CommonInfo *commonInfo; /* OPT */
	Odr_oid *schema;
	Z_InternationalString *name;
	Z_HumanString *description; /* OPT */
	int num_tagTypeMapping;
	Z_TagTypeMapping **tagTypeMapping; /* OPT */
	int num_recordStructure;
	Z_ElementInfo **recordStructure; /* OPT */
};

struct Z_ElementInfo {
	Z_InternationalString *elementName;
	Z_Path *elementTagPath;
	Z_ElementDataType *dataType; /* OPT */
	Odr_bool *required;
	Odr_bool *repeatable;
	Z_HumanString *description; /* OPT */
};

struct Z_PathUnit {
	Odr_int *tagType;
	Z_StringOrNumeric *tagValue;
};

struct Z_Path {
	int num;
	Z_PathUnit **elements;
};

struct Z_ElementInfoList {
	int num;
	Z_ElementInfo **elements;
};

struct Z_ElementDataType {
	int which;
	union {
		Z_PrimitiveDataType *primitive;
		Z_ElementInfoList *structured;
#define Z_ElementDataType_primitive 1
#define Z_ElementDataType_structured 2
	} u;
};

#define Z_PrimitiveDataType_octetString 0
#define Z_PrimitiveDataType_numeric 1
#define Z_PrimitiveDataType_date 2
#define Z_PrimitiveDataType_external 3
#define Z_PrimitiveDataType_string 4
#define Z_PrimitiveDataType_trueOrFalse 5
#define Z_PrimitiveDataType_oid 6
#define Z_PrimitiveDataType_intUnit 7
#define Z_PrimitiveDataType_empty 8
#define Z_PrimitiveDataType_noneOfTheAbove 100

struct Z_TagSetElements {
	Z_InternationalString *elementname;
	int num_nicknames;
	Z_InternationalString **nicknames; /* OPT */
	Z_StringOrNumeric *elementTag;
	Z_HumanString *description; /* OPT */
	Z_PrimitiveDataType *dataType; /* OPT */
	Z_OtherInformation *otherTagInfo; /* OPT */
};

struct Z_TagSetInfo {
	Z_CommonInfo *commonInfo; /* OPT */
	Odr_oid *tagSet;
	Z_InternationalString *name;
	Z_HumanString *description; /* OPT */
	int num_elements;
	Z_TagSetElements **elements; /* OPT */
};

struct Z_RecordSyntaxInfo {
	Z_CommonInfo *commonInfo; /* OPT */
	Odr_oid *recordSyntax;
	Z_InternationalString *name;
	int num_transferSyntaxes;
	Odr_oid **transferSyntaxes; /* OPT */
	Z_HumanString *description; /* OPT */
	Z_InternationalString *asn1Module; /* OPT */
	int num_abstractStructure;
	Z_ElementInfo **abstractStructure; /* OPT */
};

struct Z_AttributeSetInfo {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_AttributeSetId *attributeSet;
	Z_InternationalString *name;
	int num_attributes;
	Z_AttributeType **attributes; /* OPT */
	Z_HumanString *description; /* OPT */
};

struct Z_AttributeType {
	Z_InternationalString *name; /* OPT */
	Z_HumanString *description; /* OPT */
	Odr_int *attributeType;
	int num_attributeValues;
	Z_AttributeDescription **attributeValues;
};

struct Z_AttributeDescription {
	Z_InternationalString *name; /* OPT */
	Z_HumanString *description; /* OPT */
	Z_StringOrNumeric *attributeValue;
	int num_equivalentAttributes;
	Z_StringOrNumeric **equivalentAttributes; /* OPT */
};

struct Z_TermListElement {
	Z_InternationalString *name;
	Z_HumanString *title; /* OPT */
#define Z_TermListElement_optimized 0
#define Z_TermListElement_normal 1
#define Z_TermListElement_expensive 2
#define Z_TermListElement_filter 3
	Odr_int *searchCost; /* OPT */
	Odr_bool *scanable;
	int num_broader;
	Z_InternationalString **broader; /* OPT */
	int num_narrower;
	Z_InternationalString **narrower; /* OPT */
};

struct Z_TermListInfo {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_DatabaseName *databaseName;
	int num_termLists;
	Z_TermListElement **termLists;
};

struct Z_ExtendedServicesInfo {
	Z_CommonInfo *commonInfo; /* OPT */
	Odr_oid *type;
	Z_InternationalString *name; /* OPT */
	Odr_bool *privateType;
	Odr_bool *restrictionsApply;
	Odr_bool *feeApply;
	Odr_bool *available;
	Odr_bool *retentionSupported;
#define Z_ExtendedServicesInfo_waitSupported 1
#define Z_ExtendedServicesInfo_waitAlways 2
#define Z_ExtendedServicesInfo_waitNotSupported 3
#define Z_ExtendedServicesInfo_depends 4
#define Z_ExtendedServicesInfo_notSaying 5
	Odr_int *waitAction;
	Z_HumanString *description; /* OPT */
	Z_External *specificExplain; /* OPT */
	Z_InternationalString *esASN; /* OPT */
};

struct Z_AttributeDetails {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_DatabaseName *databaseName;
	int num_attributesBySet;
	Z_AttributeSetDetails **attributesBySet; /* OPT */
	Z_AttributeCombinations *attributeCombinations; /* OPT */
};

struct Z_AttributeSetDetails {
	Z_AttributeSetId *attributeSet;
	int num_attributesByType;
	Z_AttributeTypeDetails **attributesByType;
};

struct Z_AttributeTypeDetails {
	Odr_int *attributeType;
	Z_OmittedAttributeInterpretation *defaultIfOmitted; /* OPT */
	int num_attributeValues;
	Z_AttributeValue **attributeValues; /* OPT */
};

struct Z_OmittedAttributeInterpretation {
	Z_StringOrNumeric *defaultValue; /* OPT */
	Z_HumanString *defaultDescription; /* OPT */
};

struct Z_AttributeValue {
	Z_StringOrNumeric *value;
	Z_HumanString *description; /* OPT */
	int num_subAttributes;
	Z_StringOrNumeric **subAttributes; /* OPT */
	int num_superAttributes;
	Z_StringOrNumeric **superAttributes; /* OPT */
	Odr_null *partialSupport; /* OPT */
};

struct Z_EScanInfo {
	Odr_int *maxStepSize; /* OPT */
	Z_HumanString *collatingSequence; /* OPT */
	Odr_bool *increasing; /* OPT */
};

struct Z_TermListDetails {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_InternationalString *termListName;
	Z_HumanString *description; /* OPT */
	Z_AttributeCombinations *attributes; /* OPT */
	Z_EScanInfo *scanInfo; /* OPT */
	Odr_int *estNumberTerms; /* OPT */
	int num_sampleTerms;
	Z_Term **sampleTerms; /* OPT */
};

struct Z_ElementSetDetails {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_DatabaseName *databaseName;
	Z_ElementSetName *elementSetName;
	Odr_oid *recordSyntax;
	Odr_oid *schema;
	Z_HumanString *description; /* OPT */
	int num_detailsPerElement;
	Z_PerElementDetails **detailsPerElement; /* OPT */
};

struct Z_RetrievalRecordDetails {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_DatabaseName *databaseName;
	Odr_oid *schema;
	Odr_oid *recordSyntax;
	Z_HumanString *description; /* OPT */
	int num_detailsPerElement;
	Z_PerElementDetails **detailsPerElement; /* OPT */
};

struct Z_PerElementDetails {
	Z_InternationalString *name; /* OPT */
	Z_RecordTag *recordTag; /* OPT */
	int num_schemaTags;
	Z_Path **schemaTags; /* OPT */
	Odr_int *maxSize; /* OPT */
	Odr_int *minSize; /* OPT */
	Odr_int *avgSize; /* OPT */
	Odr_int *fixedSize; /* OPT */
	Odr_bool *repeatable;
	Odr_bool *required;
	Z_HumanString *description; /* OPT */
	Z_HumanString *contents; /* OPT */
	Z_HumanString *billingInfo; /* OPT */
	Z_HumanString *restrictions; /* OPT */
	int num_alternateNames;
	Z_InternationalString **alternateNames; /* OPT */
	int num_genericNames;
	Z_InternationalString **genericNames; /* OPT */
	Z_AttributeCombinations *searchAccess; /* OPT */
};

struct Z_RecordTag {
	Z_StringOrNumeric *qualifier; /* OPT */
	Z_StringOrNumeric *tagValue;
};

struct Z_SortDetails {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_DatabaseName *databaseName;
	int num_sortKeys;
	Z_SortKeyDetails **sortKeys; /* OPT */
};

struct Z_SortKeyDetails {
	Z_HumanString *description; /* OPT */
	int num_elementSpecifications;
	Z_Specification **elementSpecifications; /* OPT */
	Z_AttributeCombinations *attributeSpecifications; /* OPT */
	int which;
	union {
		Odr_null *character;
		Odr_null *numeric;
		Z_HumanString *structured;
#define Z_SortKeyDetails_character 1
#define Z_SortKeyDetails_numeric 2
#define Z_SortKeyDetails_structured 3
	} u; /* OPT */
#define Z_SortKeyDetails_always 0
#define Z_SortKeyDetails_never 1
#define Z_SortKeyDetails_default_yes 2
#define Z_SortKeyDetails_default_no 3
	Odr_int *caseSensitivity; /* OPT */
};

struct Z_ProcessingInformation {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_DatabaseName *databaseName;
#define Z_ProcessingInformation_access 0
#define Z_ProcessingInformation_search 1
#define Z_ProcessingInformation_retrieval 2
#define Z_ProcessingInformation_record_presentation 3
#define Z_ProcessingInformation_record_handling 4
	Odr_int *processingContext;
	Z_InternationalString *name;
	Odr_oid *oid;
	Z_HumanString *description; /* OPT */
	Z_External *instructions; /* OPT */
};

struct Z_VariantSetInfo {
	Z_CommonInfo *commonInfo; /* OPT */
	Odr_oid *variantSet;
	Z_InternationalString *name;
	int num_variants;
	Z_VariantClass **variants; /* OPT */
};

struct Z_VariantClass {
	Z_InternationalString *name; /* OPT */
	Z_HumanString *description; /* OPT */
	Odr_int *variantClass;
	int num_variantTypes;
	Z_VariantType **variantTypes;
};

struct Z_VariantType {
	Z_InternationalString *name; /* OPT */
	Z_HumanString *description; /* OPT */
	Odr_int *variantType;
	Z_VariantValue *variantValue; /* OPT */
};

struct Z_VariantValue {
	Z_PrimitiveDataType *dataType;
	Z_ValueSet *values; /* OPT */
};

struct Z_ValueSetEnumerated {
	int num;
	Z_ValueDescription **elements;
};

struct Z_ValueSet {
	int which;
	union {
		Z_ValueRange *range;
		Z_ValueSetEnumerated *enumerated;
#define Z_ValueSet_range 1
#define Z_ValueSet_enumerated 2
	} u;
};

struct Z_ValueRange {
	Z_ValueDescription *lower; /* OPT */
	Z_ValueDescription *upper; /* OPT */
};

struct Z_ValueDescription {
	int which;
	union {
		Odr_int *integer;
		Z_InternationalString *string;
		Odr_oct *octets;
		Odr_oid *oid;
		Z_Unit *unit;
		Z_IntUnit *valueAndUnit;
#define Z_ValueDescription_integer 1
#define Z_ValueDescription_string 2
#define Z_ValueDescription_octets 3
#define Z_ValueDescription_oid 4
#define Z_ValueDescription_unit 5
#define Z_ValueDescription_valueAndUnit 6
	} u;
};

struct Z_UnitInfo {
	Z_CommonInfo *commonInfo; /* OPT */
	Z_InternationalString *unitSystem;
	Z_HumanString *description; /* OPT */
	int num_units;
	Z_UnitType **units; /* OPT */
};

struct Z_UnitType {
	Z_InternationalString *name; /* OPT */
	Z_HumanString *description; /* OPT */
	Z_StringOrNumeric *unitType;
	int num_units;
	Z_Units **units;
};

struct Z_Units {
	Z_InternationalString *name; /* OPT */
	Z_HumanString *description; /* OPT */
	Z_StringOrNumeric *unit;
};

struct Z_CategoryList {
	Z_CommonInfo *commonInfo; /* OPT */
	int num_categories;
	Z_CategoryInfo **categories;
};

struct Z_CategoryInfo {
	Z_InternationalString *category;
	Z_InternationalString *originalCategory; /* OPT */
	Z_HumanString *description; /* OPT */
	Z_InternationalString *asn1Module; /* OPT */
};

struct Z_CommonInfo {
	char *dateAdded; /* OPT */
	char *dateChanged; /* OPT */
	char *expiry; /* OPT */
	Z_LanguageCode *humanStringLanguage; /* OPT */
	Z_OtherInformation *otherInfo; /* OPT */
};

struct Z_HumanStringUnit {
	Z_LanguageCode *language; /* OPT */
	Z_InternationalString *text;
};

struct Z_HumanString {
	int num_strings;
	Z_HumanStringUnit **strings;
};

struct Z_IconObjectUnit {
	int which;
	union {
		Z_InternationalString *ianaType;
		Z_InternationalString *z3950type;
		Z_InternationalString *otherType;
#define Z_IconObjectUnit_ianaType 1
#define Z_IconObjectUnit_z3950type 2
#define Z_IconObjectUnit_otherType 3
	} u;
	Odr_oct *content;
};

struct Z_IconObject {
	int num;
	Z_IconObjectUnit **elements;
};



struct Z_ContactInfo {
	Z_InternationalString *name; /* OPT */
	Z_HumanString *description; /* OPT */
	Z_HumanString *address; /* OPT */
	Z_InternationalString *email; /* OPT */
	Z_InternationalString *phone; /* OPT */
};

struct Z_NetworkAddressIA {
	Z_InternationalString *hostAddress;
	Odr_int *port;
};

struct Z_NetworkAddressOPA {
	Z_InternationalString *pSel;
	Z_InternationalString *sSel; /* OPT */
	Z_InternationalString *tSel; /* OPT */
	Z_InternationalString *nSap;
};

struct Z_NetworkAddressOther {
	Z_InternationalString *type;
	Z_InternationalString *address;
};

struct Z_NetworkAddress {
	int which;
	union {
		Z_NetworkAddressIA *internetAddress;
		Z_NetworkAddressOPA *osiPresentationAddress;
		Z_NetworkAddressOther *other;
#define Z_NetworkAddress_iA 1
#define Z_NetworkAddress_oPA 2
#define Z_NetworkAddress_other 3
	} u;
};

struct Z_AccessInfo {
	int num_queryTypesSupported;
	Z_QueryTypeDetails **queryTypesSupported; /* OPT */
	int num_diagnosticsSets;
	Odr_oid **diagnosticsSets; /* OPT */
	int num_attributeSetIds;
	Z_AttributeSetId **attributeSetIds; /* OPT */
	int num_schemas;
	Odr_oid **schemas; /* OPT */
	int num_recordSyntaxes;
	Odr_oid **recordSyntaxes; /* OPT */
	int num_resourceChallenges;
	Odr_oid **resourceChallenges; /* OPT */
	Z_AccessRestrictions *restrictedAccess; /* OPT */
	Z_Costs *costInfo; /* OPT */
	int num_variantSets;
	Odr_oid **variantSets; /* OPT */
	int num_elementSetNames;
	Z_ElementSetName **elementSetNames; /* OPT */
	int num_unitSystems;
	Z_InternationalString **unitSystems;
};

struct Z_QueryTypeDetails {
	int which;
	union {
		Z_PrivateCapabilities *zprivate;
		Z_RpnCapabilities *rpn;
		Z_Iso8777Capabilities *iso8777;
		Z_HumanString *z39_58;
		Z_RpnCapabilities *erpn;
		Z_HumanString *rankedList;
#define Z_QueryTypeDetails_private 1
#define Z_QueryTypeDetails_rpn 2
#define Z_QueryTypeDetails_iso8777 3
#define Z_QueryTypeDetails_z39_58 4
#define Z_QueryTypeDetails_erpn 5
#define Z_QueryTypeDetails_rankedList 6
	} u;
};

struct Z_PrivateCapOperator {
	Z_InternationalString *roperator;
	Z_HumanString *description; /* OPT */
};

struct Z_PrivateCapabilities {
	int num_operators;
	Z_PrivateCapOperator **operators; /* OPT */
	int num_searchKeys;
	Z_SearchKey **searchKeys; /* OPT */
	int num_description;
	Z_HumanString **description; /* OPT */
};

struct Z_RpnCapabilities {
	int num_operators;
	Odr_int **operators; /* OPT */
	Odr_bool *resultSetAsOperandSupported;
	Odr_bool *restrictionOperandSupported;
	Z_ProximitySupport *proximity; /* OPT */
};

struct Z_Iso8777Capabilities {
	int num_searchKeys;
	Z_SearchKey **searchKeys;
	Z_HumanString *restrictions; /* OPT */
};

struct Z_ProxSupportPrivate {
	Odr_int *unit;
	Z_HumanString *description; /* OPT */
};

struct Z_ProxSupportUnit {
	int which;
	union {
		Odr_int *known;
		Z_ProxSupportPrivate *zprivate;
#define Z_ProxSupportUnit_known 1
#define Z_ProxSupportUnit_private 2
	} u;
};

struct Z_ProximitySupport {
	Odr_bool *anySupport;
	int num_unitsSupported;
	Z_ProxSupportUnit **unitsSupported; /* OPT */
};

struct Z_SearchKey {
	Z_InternationalString *searchKey;
	Z_HumanString *description; /* OPT */
};

struct Z_AccessRestrictionsUnit {
#define Z_AccessRestrictionsUnit_any 0
#define Z_AccessRestrictionsUnit_search 1
#define Z_AccessRestrictionsUnit_present 2
#define Z_AccessRestrictionsUnit_specific_elements 3
#define Z_AccessRestrictionsUnit_extended_services 4
#define Z_AccessRestrictionsUnit_by_database 5
	Odr_int *accessType;
	Z_HumanString *accessText; /* OPT */
	int num_accessChallenges;
	Odr_oid **accessChallenges; /* OPT */
};

struct Z_AccessRestrictions {
	int num;
	Z_AccessRestrictionsUnit **elements;
};

struct Z_CostsOtherCharge {
	Z_HumanString *forWhat;
	Z_Charge *charge;
};

struct Z_Costs {
	Z_Charge *connectCharge; /* OPT */
	Z_Charge *connectTime; /* OPT */
	Z_Charge *displayCharge; /* OPT */
	Z_Charge *searchCharge; /* OPT */
	Z_Charge *subscriptCharge; /* OPT */
	int num_otherCharges;
	Z_CostsOtherCharge **otherCharges; /* OPT */
};

struct Z_Charge {
	Z_IntUnit *cost;
	Z_Unit *perWhat; /* OPT */
	Z_HumanString *text; /* OPT */
};

struct Z_DatabaseList {
	int num_databases;
	Z_DatabaseName **databases;
};

struct Z_AttributeCombinations {
	Z_AttributeSetId *defaultAttributeSet;
	int num_legalCombinations;
	Z_AttributeCombination **legalCombinations;
};

struct Z_AttributeCombination {
	int num_occurrences;
	Z_AttributeOccurrence **occurrences;
};

struct Z_AttributeValueList {
	int num_attributes;
	Z_StringOrNumeric **attributes;
};

struct Z_AttributeOccurrence {
	Z_AttributeSetId *attributeSet; /* OPT */
	Odr_int *attributeType;
	Odr_null *mustBeSupplied; /* OPT */
	int which;
	union {
		Odr_null *any_or_none;
		Z_AttributeValueList *specific;
#define Z_AttributeOcc_any_or_none 1
#define Z_AttributeOcc_specific 2
	} attributeValues;
};

#ifdef __cplusplus
}
#endif
#endif
