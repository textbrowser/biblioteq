/** \file item-req.h
    \brief ASN.1 Module Z39.50-extendedService-ItemOrder-ItemRequest-1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef item_req_H
#define item_req_H

#include <yaz/odr.h>
#include <yaz/ill-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct ILL_ItemRequest ILL_ItemRequest;
YAZ_EXPORT int ill_ItemRequest(ODR o, ILL_ItemRequest **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct ILL_ItemRequest {
#define ILL_ItemRequest_version_1 1
#define ILL_ItemRequest_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id; /* OPT */
	ILL_Service_Date_Time *service_date_time; /* OPT */
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_Transaction_Type *transaction_type;
	ILL_Delivery_Address *delivery_address; /* OPT */
	ILL_Delivery_Service *delivery_service; /* OPT */
	ILL_Delivery_Address *billing_address; /* OPT */
	int num_iLL_service_type;
	ILL_Service_Type **iLL_service_type; /* OPT */
	Odr_external *responder_specific_service; /* OPT */
	ILL_Requester_Optional_Messages_Type *requester_optional_messages; /* OPT */
	ILL_Search_Type *search_type; /* OPT */
	int num_supply_medium_info_type;
	ILL_Supply_Medium_Info_Type **supply_medium_info_type; /* OPT */
	ILL_Place_On_Hold_Type *place_on_hold;
	ILL_Client_Id *client_id; /* OPT */
	ILL_Item_Id *item_id; /* OPT */
	ILL_Supplemental_Item_Description *supplemental_item_description; /* OPT */
	ILL_Cost_Info_Type *cost_info_type; /* OPT */
	ILL_String *copyright_compliance; /* OPT */
	ILL_Third_Party_Info_Type *third_party_info_type; /* OPT */
	Odr_bool *retry_flag;
	Odr_bool *forward_flag;
	ILL_String *requester_note; /* OPT */
	ILL_String *forward_note; /* OPT */
	int num_iLL_request_extensions;
	ILL_Extension **iLL_request_extensions; /* OPT */
};

#ifdef __cplusplus
}
#endif
#endif
