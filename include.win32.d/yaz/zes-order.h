/** \file zes-order.h
    \brief ASN.1 Module ESFormat-ItemOrder

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef zes_order_H
#define zes_order_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_IORequest Z_IORequest;
YAZ_EXPORT int z_IORequest(ODR o, Z_IORequest **p, int opt, const char *name);

typedef struct Z_IOTaskPackage Z_IOTaskPackage;
YAZ_EXPORT int z_IOTaskPackage(ODR o, Z_IOTaskPackage **p, int opt, const char *name);

typedef struct Z_IOItemOrder Z_IOItemOrder;
YAZ_EXPORT int z_IOItemOrder(ODR o, Z_IOItemOrder **p, int opt, const char *name);

typedef struct Z_IOContact Z_IOContact;
YAZ_EXPORT int z_IOContact(ODR o, Z_IOContact **p, int opt, const char *name);

typedef struct Z_IOBilling Z_IOBilling;
YAZ_EXPORT int z_IOBilling(ODR o, Z_IOBilling **p, int opt, const char *name);

typedef struct Z_IOOriginPartToKeep Z_IOOriginPartToKeep;
YAZ_EXPORT int z_IOOriginPartToKeep(ODR o, Z_IOOriginPartToKeep **p, int opt, const char *name);

typedef struct Z_IOCreditCardInfo Z_IOCreditCardInfo;
YAZ_EXPORT int z_IOCreditCardInfo(ODR o, Z_IOCreditCardInfo **p, int opt, const char *name);

typedef struct Z_IOResultSetItem Z_IOResultSetItem;
YAZ_EXPORT int z_IOResultSetItem(ODR o, Z_IOResultSetItem **p, int opt, const char *name);

typedef struct Z_IOOriginPartNotToKeep Z_IOOriginPartNotToKeep;
YAZ_EXPORT int z_IOOriginPartNotToKeep(ODR o, Z_IOOriginPartNotToKeep **p, int opt, const char *name);

typedef struct Z_IOTargetPart Z_IOTargetPart;
YAZ_EXPORT int z_IOTargetPart(ODR o, Z_IOTargetPart **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_IORequest {
	Z_IOOriginPartToKeep *toKeep; /* OPT */
	Z_IOOriginPartNotToKeep *notToKeep;
};

struct Z_IOTaskPackage {
	Z_IOOriginPartToKeep *originPart; /* OPT */
	Z_IOTargetPart *targetPart;
};

struct Z_IOItemOrder {
	int which;
	union {
		Z_IORequest *esRequest;
		Z_IOTaskPackage *taskPackage;
#define Z_IOItemOrder_esRequest 1
#define Z_IOItemOrder_taskPackage 2
	} u;
};

struct Z_IOContact {
	Z_InternationalString *name; /* OPT */
	Z_InternationalString *phone; /* OPT */
	Z_InternationalString *email; /* OPT */
};

struct Z_IOBilling {
	int which;
	union {
		Odr_null *billInvoice;
		Odr_null *prepay;
		Odr_null *depositAccount;
		Z_IOCreditCardInfo *creditCard;
		Odr_null *cardInfoPreviouslySupplied;
		Odr_null *privateKnown;
		Z_External *privateNotKnown;
#define Z_IOBilling_billInvoice 1
#define Z_IOBilling_prepay 2
#define Z_IOBilling_depositAccount 3
#define Z_IOBilling_creditCard 4
#define Z_IOBilling_cardInfoPreviouslySupplied 5
#define Z_IOBilling_privateKnown 6
#define Z_IOBilling_privateNotKnown 7
	} u;
	Z_InternationalString *customerReference; /* OPT */
	Z_InternationalString *customerPONumber; /* OPT */
};

struct Z_IOOriginPartToKeep {
	Z_External *supplDescription; /* OPT */
	Z_IOContact *contact; /* OPT */
	Z_IOBilling *addlBilling; /* OPT */
};

struct Z_IOCreditCardInfo {
	Z_InternationalString *nameOnCard;
	Z_InternationalString *expirationDate;
	Z_InternationalString *cardNumber;
};

struct Z_IOResultSetItem {
	Z_InternationalString *resultSetId;
	Odr_int *item;
};

struct Z_IOOriginPartNotToKeep {
	Z_IOResultSetItem *resultSetItem; /* OPT */
	Z_External *itemRequest; /* OPT */
};

struct Z_IOTargetPart {
	Z_External *itemRequest; /* OPT */
	Z_External *statusOrErrorReport; /* OPT */
#define Z_IOTargetPart_notReceived 1
#define Z_IOTargetPart_loanQueue 2
#define Z_IOTargetPart_forwarded 3
#define Z_IOTargetPart_unfilledCopyright 4
#define Z_IOTargetPart_filledCopyright 5
	Odr_int *auxiliaryStatus; /* OPT */
};

#ifdef __cplusplus
}
#endif
#endif
