/** \file ill-core.h
    \brief ASN.1 Module ISO-10161-ILL-1

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef ill_core_H
#define ill_core_H

#include <yaz/odr.h>
#ifdef __cplusplus
extern "C" {
#endif



typedef struct ILL_APDU ILL_APDU;
YAZ_EXPORT int ill_APDU(ODR o, ILL_APDU **p, int opt, const char *name);

typedef struct ILL_Request ILL_Request;
YAZ_EXPORT int ill_Request(ODR o, ILL_Request **p, int opt, const char *name);

typedef struct ILL_Forward_Notification ILL_Forward_Notification;
YAZ_EXPORT int ill_Forward_Notification(ODR o, ILL_Forward_Notification **p, int opt, const char *name);

typedef struct ILL_Shipped ILL_Shipped;
YAZ_EXPORT int ill_Shipped(ODR o, ILL_Shipped **p, int opt, const char *name);

typedef struct ILL_Answer ILL_Answer;
YAZ_EXPORT int ill_Answer(ODR o, ILL_Answer **p, int opt, const char *name);

typedef struct ILL_Conditional_Reply ILL_Conditional_Reply;
YAZ_EXPORT int ill_Conditional_Reply(ODR o, ILL_Conditional_Reply **p, int opt, const char *name);

typedef struct ILL_Cancel ILL_Cancel;
YAZ_EXPORT int ill_Cancel(ODR o, ILL_Cancel **p, int opt, const char *name);

typedef struct ILL_Cancel_Reply ILL_Cancel_Reply;
YAZ_EXPORT int ill_Cancel_Reply(ODR o, ILL_Cancel_Reply **p, int opt, const char *name);

typedef struct ILL_Received ILL_Received;
YAZ_EXPORT int ill_Received(ODR o, ILL_Received **p, int opt, const char *name);

typedef struct ILL_Recall ILL_Recall;
YAZ_EXPORT int ill_Recall(ODR o, ILL_Recall **p, int opt, const char *name);

typedef struct ILL_Returned ILL_Returned;
YAZ_EXPORT int ill_Returned(ODR o, ILL_Returned **p, int opt, const char *name);

typedef struct ILL_Checked_In ILL_Checked_In;
YAZ_EXPORT int ill_Checked_In(ODR o, ILL_Checked_In **p, int opt, const char *name);

typedef struct ILL_Overdue_ExtensionS ILL_Overdue_ExtensionS;
YAZ_EXPORT int ill_Overdue_ExtensionS(ODR o, ILL_Overdue_ExtensionS **p, int opt, const char *name);

typedef struct ILL_Overdue ILL_Overdue;
YAZ_EXPORT int ill_Overdue(ODR o, ILL_Overdue **p, int opt, const char *name);

typedef struct ILL_Renew ILL_Renew;
YAZ_EXPORT int ill_Renew(ODR o, ILL_Renew **p, int opt, const char *name);

typedef struct ILL_Renew_Answer ILL_Renew_Answer;
YAZ_EXPORT int ill_Renew_Answer(ODR o, ILL_Renew_Answer **p, int opt, const char *name);

typedef struct ILL_Lost ILL_Lost;
YAZ_EXPORT int ill_Lost(ODR o, ILL_Lost **p, int opt, const char *name);

typedef struct ILL_Damaged ILL_Damaged;
YAZ_EXPORT int ill_Damaged(ODR o, ILL_Damaged **p, int opt, const char *name);

typedef struct ILL_Message ILL_Message;
YAZ_EXPORT int ill_Message(ODR o, ILL_Message **p, int opt, const char *name);

typedef struct ILL_Status_Query ILL_Status_Query;
YAZ_EXPORT int ill_Status_Query(ODR o, ILL_Status_Query **p, int opt, const char *name);

typedef struct ILL_Status_Or_Error_Report ILL_Status_Or_Error_Report;
YAZ_EXPORT int ill_Status_Or_Error_Report(ODR o, ILL_Status_Or_Error_Report **p, int opt, const char *name);

typedef struct ILL_Expired ILL_Expired;
YAZ_EXPORT int ill_Expired(ODR o, ILL_Expired **p, int opt, const char *name);

typedef struct ILL_Already_Forwarded ILL_Already_Forwarded;
YAZ_EXPORT int ill_Already_Forwarded(ODR o, ILL_Already_Forwarded **p, int opt, const char *name);

typedef struct ILL_Already_Tried_List_Type ILL_Already_Tried_List_Type;
YAZ_EXPORT int ill_Already_Tried_List_Type(ODR o, ILL_Already_Tried_List_Type **p, int opt, const char *name);

typedef struct ILL_Amount ILL_Amount;
YAZ_EXPORT int ill_Amount(ODR o, ILL_Amount **p, int opt, const char *name);

typedef char  ILL_AmountString;
YAZ_EXPORT int ill_AmountString(ODR o, ILL_AmountString **p, int opt, const char *name);

typedef struct ILL_Client_Id ILL_Client_Id;
YAZ_EXPORT int ill_Client_Id(ODR o, ILL_Client_Id **p, int opt, const char *name);

typedef struct ILL_Conditional_Results ILL_Conditional_Results;
YAZ_EXPORT int ill_Conditional_Results(ODR o, ILL_Conditional_Results **p, int opt, const char *name);

typedef struct ILL_Cost_Info_Type ILL_Cost_Info_Type;
YAZ_EXPORT int ill_Cost_Info_Type(ODR o, ILL_Cost_Info_Type **p, int opt, const char *name);

typedef Odr_int  ILL_Current_State;
YAZ_EXPORT int ill_Current_State(ODR o, ILL_Current_State **p, int opt, const char *name);

typedef struct ILL_Damaged_DetailsSpecific_units ILL_Damaged_DetailsSpecific_units;
YAZ_EXPORT int ill_Damaged_DetailsSpecific_units(ODR o, ILL_Damaged_DetailsSpecific_units **p, int opt, const char *name);

typedef struct ILL_Damaged_Details ILL_Damaged_Details;
YAZ_EXPORT int ill_Damaged_Details(ODR o, ILL_Damaged_Details **p, int opt, const char *name);

typedef struct ILL_Date_Due ILL_Date_Due;
YAZ_EXPORT int ill_Date_Due(ODR o, ILL_Date_Due **p, int opt, const char *name);

typedef struct ILL_Delivery_Address ILL_Delivery_Address;
YAZ_EXPORT int ill_Delivery_Address(ODR o, ILL_Delivery_Address **p, int opt, const char *name);

typedef struct ILL_Delivery_ServiceElectronic_delivery ILL_Delivery_ServiceElectronic_delivery;
YAZ_EXPORT int ill_Delivery_ServiceElectronic_delivery(ODR o, ILL_Delivery_ServiceElectronic_delivery **p, int opt, const char *name);

typedef struct ILL_Delivery_Service ILL_Delivery_Service;
YAZ_EXPORT int ill_Delivery_Service(ODR o, ILL_Delivery_Service **p, int opt, const char *name);

typedef struct ILL_Electronic_Delivery_Service_0 ILL_Electronic_Delivery_Service_0;
YAZ_EXPORT int ill_Electronic_Delivery_Service_0(ODR o, ILL_Electronic_Delivery_Service_0 **p, int opt, const char *name);

typedef struct ILL_Electronic_Delivery_Service_1 ILL_Electronic_Delivery_Service_1;
YAZ_EXPORT int ill_Electronic_Delivery_Service_1(ODR o, ILL_Electronic_Delivery_Service_1 **p, int opt, const char *name);

typedef struct ILL_Electronic_Delivery_Service ILL_Electronic_Delivery_Service;
YAZ_EXPORT int ill_Electronic_Delivery_Service(ODR o, ILL_Electronic_Delivery_Service **p, int opt, const char *name);

typedef struct ILL_Error_Report ILL_Error_Report;
YAZ_EXPORT int ill_Error_Report(ODR o, ILL_Error_Report **p, int opt, const char *name);

typedef struct ILL_Estimate_Results ILL_Estimate_Results;
YAZ_EXPORT int ill_Estimate_Results(ODR o, ILL_Estimate_Results **p, int opt, const char *name);

typedef struct ILL_Extension ILL_Extension;
YAZ_EXPORT int ill_Extension(ODR o, ILL_Extension **p, int opt, const char *name);

typedef Odr_int  ILL_General_Problem;
YAZ_EXPORT int ill_General_Problem(ODR o, ILL_General_Problem **p, int opt, const char *name);

typedef struct ILL_History_Report ILL_History_Report;
YAZ_EXPORT int ill_History_Report(ODR o, ILL_History_Report **p, int opt, const char *name);

typedef struct ILL_Hold_Placed_Results ILL_Hold_Placed_Results;
YAZ_EXPORT int ill_Hold_Placed_Results(ODR o, ILL_Hold_Placed_Results **p, int opt, const char *name);

typedef Odr_int  ILL_APDU_Type;
YAZ_EXPORT int ill_APDU_Type(ODR o, ILL_APDU_Type **p, int opt, const char *name);

typedef Odr_int  ILL_Service_Type;
YAZ_EXPORT int ill_Service_Type(ODR o, ILL_Service_Type **p, int opt, const char *name);

typedef struct ILL_String ILL_String;
YAZ_EXPORT int ill_String(ODR o, ILL_String **p, int opt, const char *name);

typedef ILL_String  ILL_Account_Number;
YAZ_EXPORT int ill_Account_Number(ODR o, ILL_Account_Number **p, int opt, const char *name);

typedef Odr_int  ILL_Intermediary_Problem;
YAZ_EXPORT int ill_Intermediary_Problem(ODR o, ILL_Intermediary_Problem **p, int opt, const char *name);

typedef char  ILL_ISO_Date;
YAZ_EXPORT int ill_ISO_Date(ODR o, ILL_ISO_Date **p, int opt, const char *name);

typedef char  ILL_ISO_Time;
YAZ_EXPORT int ill_ISO_Time(ODR o, ILL_ISO_Time **p, int opt, const char *name);

typedef struct ILL_Item_Id ILL_Item_Id;
YAZ_EXPORT int ill_Item_Id(ODR o, ILL_Item_Id **p, int opt, const char *name);

typedef struct ILL_Location_Info ILL_Location_Info;
YAZ_EXPORT int ill_Location_Info(ODR o, ILL_Location_Info **p, int opt, const char *name);

typedef struct ILL_Locations_Results ILL_Locations_Results;
YAZ_EXPORT int ill_Locations_Results(ODR o, ILL_Locations_Results **p, int opt, const char *name);

typedef Odr_int  ILL_Medium_Type;
YAZ_EXPORT int ill_Medium_Type(ODR o, ILL_Medium_Type **p, int opt, const char *name);

typedef struct ILL_Name_Of_Person_Or_Institution ILL_Name_Of_Person_Or_Institution;
YAZ_EXPORT int ill_Name_Of_Person_Or_Institution(ODR o, ILL_Name_Of_Person_Or_Institution **p, int opt, const char *name);

typedef struct ILL_Person_Or_Institution_Symbol ILL_Person_Or_Institution_Symbol;
YAZ_EXPORT int ill_Person_Or_Institution_Symbol(ODR o, ILL_Person_Or_Institution_Symbol **p, int opt, const char *name);

typedef Odr_int  ILL_Place_On_Hold_Type;
YAZ_EXPORT int ill_Place_On_Hold_Type(ODR o, ILL_Place_On_Hold_Type **p, int opt, const char *name);

typedef struct ILL_Postal_Address ILL_Postal_Address;
YAZ_EXPORT int ill_Postal_Address(ODR o, ILL_Postal_Address **p, int opt, const char *name);

typedef struct ILL_Provider_Error_Report ILL_Provider_Error_Report;
YAZ_EXPORT int ill_Provider_Error_Report(ODR o, ILL_Provider_Error_Report **p, int opt, const char *name);

typedef Odr_int  ILL_Reason_Locs_Provided;
YAZ_EXPORT int ill_Reason_Locs_Provided(ODR o, ILL_Reason_Locs_Provided **p, int opt, const char *name);

typedef Odr_int  ILL_Reason_No_Report;
YAZ_EXPORT int ill_Reason_No_Report(ODR o, ILL_Reason_No_Report **p, int opt, const char *name);

typedef Odr_int  ILL_Reason_Unfilled;
YAZ_EXPORT int ill_Reason_Unfilled(ODR o, ILL_Reason_Unfilled **p, int opt, const char *name);

typedef Odr_int  ILL_Report_Source;
YAZ_EXPORT int ill_Report_Source(ODR o, ILL_Report_Source **p, int opt, const char *name);

typedef struct ILL_Requester_Optional_Messages_Type ILL_Requester_Optional_Messages_Type;
YAZ_EXPORT int ill_Requester_Optional_Messages_Type(ODR o, ILL_Requester_Optional_Messages_Type **p, int opt, const char *name);

typedef struct ILL_Responder_Optional_Messages_Type ILL_Responder_Optional_Messages_Type;
YAZ_EXPORT int ill_Responder_Optional_Messages_Type(ODR o, ILL_Responder_Optional_Messages_Type **p, int opt, const char *name);

typedef struct ILL_Retry_Results ILL_Retry_Results;
YAZ_EXPORT int ill_Retry_Results(ODR o, ILL_Retry_Results **p, int opt, const char *name);

typedef struct ILL_Search_Type ILL_Search_Type;
YAZ_EXPORT int ill_Search_Type(ODR o, ILL_Search_Type **p, int opt, const char *name);

typedef ILL_String  ILL_Security_Problem;
YAZ_EXPORT int ill_Security_Problem(ODR o, ILL_Security_Problem **p, int opt, const char *name);

typedef struct ILL_Send_To_List_Type_s ILL_Send_To_List_Type_s;
YAZ_EXPORT int ill_Send_To_List_Type_s(ODR o, ILL_Send_To_List_Type_s **p, int opt, const char *name);

typedef struct ILL_Send_To_List_Type ILL_Send_To_List_Type;
YAZ_EXPORT int ill_Send_To_List_Type(ODR o, ILL_Send_To_List_Type **p, int opt, const char *name);

typedef struct ILL_Service_Date_this ILL_Service_Date_this;
YAZ_EXPORT int ill_Service_Date_this(ODR o, ILL_Service_Date_this **p, int opt, const char *name);

typedef struct ILL_Service_Date_original ILL_Service_Date_original;
YAZ_EXPORT int ill_Service_Date_original(ODR o, ILL_Service_Date_original **p, int opt, const char *name);

typedef struct ILL_Service_Date_Time ILL_Service_Date_Time;
YAZ_EXPORT int ill_Service_Date_Time(ODR o, ILL_Service_Date_Time **p, int opt, const char *name);

typedef ILL_Service_Type  ILL_Shipped_Service_Type;
YAZ_EXPORT int ill_Shipped_Service_Type(ODR o, ILL_Shipped_Service_Type **p, int opt, const char *name);

typedef struct ILL_State_Transition_Prohibited ILL_State_Transition_Prohibited;
YAZ_EXPORT int ill_State_Transition_Prohibited(ODR o, ILL_State_Transition_Prohibited **p, int opt, const char *name);

typedef struct ILL_Status_Report ILL_Status_Report;
YAZ_EXPORT int ill_Status_Report(ODR o, ILL_Status_Report **p, int opt, const char *name);

typedef struct ILL_Supplemental_Item_Description ILL_Supplemental_Item_Description;
YAZ_EXPORT int ill_Supplemental_Item_Description(ODR o, ILL_Supplemental_Item_Description **p, int opt, const char *name);

typedef struct ILL_Supply_Details ILL_Supply_Details;
YAZ_EXPORT int ill_Supply_Details(ODR o, ILL_Supply_Details **p, int opt, const char *name);

typedef struct ILL_Supply_Medium_Info_Type ILL_Supply_Medium_Info_Type;
YAZ_EXPORT int ill_Supply_Medium_Info_Type(ODR o, ILL_Supply_Medium_Info_Type **p, int opt, const char *name);

typedef Odr_int  ILL_Supply_Medium_Type;
YAZ_EXPORT int ill_Supply_Medium_Type(ODR o, ILL_Supply_Medium_Type **p, int opt, const char *name);

typedef struct ILL_System_Address ILL_System_Address;
YAZ_EXPORT int ill_System_Address(ODR o, ILL_System_Address **p, int opt, const char *name);

typedef struct ILL_System_Id ILL_System_Id;
YAZ_EXPORT int ill_System_Id(ODR o, ILL_System_Id **p, int opt, const char *name);

typedef struct ILL_Third_Party_Info_Type ILL_Third_Party_Info_Type;
YAZ_EXPORT int ill_Third_Party_Info_Type(ODR o, ILL_Third_Party_Info_Type **p, int opt, const char *name);

typedef struct ILL_Transaction_Id ILL_Transaction_Id;
YAZ_EXPORT int ill_Transaction_Id(ODR o, ILL_Transaction_Id **p, int opt, const char *name);

typedef Odr_int  ILL_Transaction_Id_Problem;
YAZ_EXPORT int ill_Transaction_Id_Problem(ODR o, ILL_Transaction_Id_Problem **p, int opt, const char *name);

typedef Odr_int  ILL_Transaction_Results;
YAZ_EXPORT int ill_Transaction_Results(ODR o, ILL_Transaction_Results **p, int opt, const char *name);

typedef Odr_int  ILL_Transaction_Type;
YAZ_EXPORT int ill_Transaction_Type(ODR o, ILL_Transaction_Type **p, int opt, const char *name);

typedef ILL_String  ILL_Transportation_Mode;
YAZ_EXPORT int ill_Transportation_Mode(ODR o, ILL_Transportation_Mode **p, int opt, const char *name);

typedef Odr_int  ILL_Unable_To_Perform;
YAZ_EXPORT int ill_Unable_To_Perform(ODR o, ILL_Unable_To_Perform **p, int opt, const char *name);

typedef struct ILL_Unfilled_Results ILL_Unfilled_Results;
YAZ_EXPORT int ill_Unfilled_Results(ODR o, ILL_Unfilled_Results **p, int opt, const char *name);

typedef struct ILL_Units_Per_Medium_Type ILL_Units_Per_Medium_Type;
YAZ_EXPORT int ill_Units_Per_Medium_Type(ODR o, ILL_Units_Per_Medium_Type **p, int opt, const char *name);

typedef struct ILL_User_Error_Report ILL_User_Error_Report;
YAZ_EXPORT int ill_User_Error_Report(ODR o, ILL_User_Error_Report **p, int opt, const char *name);

typedef struct ILL_Will_Supply_Results ILL_Will_Supply_Results;
YAZ_EXPORT int ill_Will_Supply_Results(ODR o, ILL_Will_Supply_Results **p, int opt, const char *name);

typedef char  ILL_EDIFACTString;
YAZ_EXPORT int ill_EDIFACTString(ODR o, ILL_EDIFACTString **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct ILL_APDU {
	int which;
	union {
		ILL_Request *illRequest;
		ILL_Forward_Notification *Forward_Notification;
		ILL_Shipped *Shipped;
		ILL_Answer *illAnswer;
		ILL_Conditional_Reply *Conditional_Reply;
		ILL_Cancel *Cancel;
		ILL_Cancel_Reply *Cancel_Reply;
		ILL_Received *Received;
		ILL_Recall *Recall;
		ILL_Returned *Returned;
		ILL_Checked_In *Checked_In;
		ILL_Overdue *Overdue;
		ILL_Renew *Renew;
		ILL_Renew_Answer *Renew_Answer;
		ILL_Lost *Lost;
		ILL_Damaged *Damaged;
		ILL_Message *Message;
		ILL_Status_Query *Status_Query;
		ILL_Status_Or_Error_Report *Status_Or_Error_Report;
		ILL_Expired *Expired;
#define ILL_APDU_ILL_Request 1
#define ILL_APDU_Forward_Notification 2
#define ILL_APDU_Shipped 3
#define ILL_APDU_ILL_Answer 4
#define ILL_APDU_Conditional_Reply 5
#define ILL_APDU_Cancel 6
#define ILL_APDU_Cancel_Reply 7
#define ILL_APDU_Received 8
#define ILL_APDU_Recall 9
#define ILL_APDU_Returned 10
#define ILL_APDU_Checked_In 11
#define ILL_APDU_Overdue 12
#define ILL_APDU_Renew 13
#define ILL_APDU_Renew_Answer 14
#define ILL_APDU_Lost 15
#define ILL_APDU_Damaged 16
#define ILL_APDU_Message 17
#define ILL_APDU_Status_Query 18
#define ILL_APDU_Status_Or_Error_Report 19
#define ILL_APDU_Expired 20
	} u;
};

struct ILL_Request {
#define ILL_Request_version_1 1
#define ILL_Request_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_Transaction_Type *transaction_type;
	ILL_Delivery_Address *delivery_address; /* OPT */
	ILL_Delivery_Service *delivery_service; /* OPT */
	ILL_Delivery_Address *billing_address; /* OPT */
	int num_iLL_service_type;
	ILL_Service_Type **iLL_service_type;
	Odr_external *responder_specific_service; /* OPT */
	ILL_Requester_Optional_Messages_Type *requester_optional_messages;
	ILL_Search_Type *search_type; /* OPT */
	int num_supply_medium_info_type;
	ILL_Supply_Medium_Info_Type **supply_medium_info_type; /* OPT */
	ILL_Place_On_Hold_Type *place_on_hold;
	ILL_Client_Id *client_id; /* OPT */
	ILL_Item_Id *item_id;
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

struct ILL_Forward_Notification {
#define ILL_Forward_Notification_version_1 1
#define ILL_Forward_Notification_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id;
	ILL_System_Address *responder_address; /* OPT */
	ILL_System_Id *intermediary_id;
	ILL_String *notification_note; /* OPT */
	int num_forward_notification_extensions;
	ILL_Extension **forward_notification_extensions; /* OPT */
};

struct ILL_Shipped {
#define ILL_Shipped_version_1 1
#define ILL_Shipped_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_System_Address *responder_address; /* OPT */
	ILL_System_Id *intermediary_id; /* OPT */
	ILL_System_Id *supplier_id; /* OPT */
	ILL_Client_Id *client_id; /* OPT */
	ILL_Transaction_Type *transaction_type;
	ILL_Supplemental_Item_Description *supplemental_item_description; /* OPT */
	ILL_Shipped_Service_Type *shipped_service_type;
	ILL_Responder_Optional_Messages_Type *responder_optional_messages; /* OPT */
	ILL_Supply_Details *supply_details;
	ILL_Postal_Address *return_to_address; /* OPT */
	ILL_String *responder_note; /* OPT */
	int num_shipped_extensions;
	ILL_Extension **shipped_extensions; /* OPT */
};

struct ILL_Answer {
#define ILL_Answer_version_1 1
#define ILL_Answer_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_Transaction_Results *transaction_results;
	int which;
	union {
		ILL_Conditional_Results *conditional_results;
		ILL_Retry_Results *retry_results;
		ILL_Unfilled_Results *unfilled_results;
		ILL_Locations_Results *locations_results;
		ILL_Will_Supply_Results *will_supply_results;
		ILL_Hold_Placed_Results *hold_placed_results;
		ILL_Estimate_Results *estimate_results;
#define ILL_Answer_conditional_results 1
#define ILL_Answer_retry_results 2
#define ILL_Answer_unfilled_results 3
#define ILL_Answer_locations_results 4
#define ILL_Answer_will_supply_results 5
#define ILL_Answer_hold_placed_results 6
#define ILL_Answer_estimate_results 7
	} u; /* OPT */
	Odr_external *responder_specific_results; /* OPT */
	ILL_Supplemental_Item_Description *supplemental_item_description; /* OPT */
	ILL_Send_To_List_Type *send_to_list; /* OPT */
	ILL_Already_Tried_List_Type *already_tried_list; /* OPT */
	ILL_Responder_Optional_Messages_Type *responder_optional_messages; /* OPT */
	ILL_String *responder_note; /* OPT */
	int num_ill_answer_extensions;
	ILL_Extension **ill_answer_extensions; /* OPT */
};

struct ILL_Conditional_Reply {
#define ILL_Conditional_Reply_version_1 1
#define ILL_Conditional_Reply_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	Odr_bool *answer;
	ILL_String *requester_note; /* OPT */
	int num_conditional_reply_extensions;
	ILL_Extension **conditional_reply_extensions; /* OPT */
};

struct ILL_Cancel {
#define ILL_Cancel_version_1 1
#define ILL_Cancel_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_String *requester_note; /* OPT */
	int num_cancel_extensions;
	ILL_Extension **cancel_extensions; /* OPT */
};

struct ILL_Cancel_Reply {
#define ILL_Cancel_Reply_version_1 1
#define ILL_Cancel_Reply_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	Odr_bool *answer;
	ILL_String *responder_note; /* OPT */
	int num_cancel_reply_extensions;
	ILL_Extension **cancel_reply_extensions; /* OPT */
};

struct ILL_Received {
#define ILL_Received_version_1 1
#define ILL_Received_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_System_Id *supplier_id; /* OPT */
	ILL_Supplemental_Item_Description *supplemental_item_description; /* OPT */
	ILL_ISO_Date *date_received;
	ILL_Shipped_Service_Type *shipped_service_type;
	ILL_String *requester_note; /* OPT */
	int num_received_extensions;
	ILL_Extension **received_extensions; /* OPT */
};

struct ILL_Recall {
#define ILL_Recall_version_1 1
#define ILL_Recall_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_String *responder_note; /* OPT */
	int num_recall_extensions;
	ILL_Extension **recall_extensions; /* OPT */
};

struct ILL_Returned {
#define ILL_Returned_version_1 1
#define ILL_Returned_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_Supplemental_Item_Description *supplemental_item_description; /* OPT */
	ILL_ISO_Date *date_returned;
	ILL_Transportation_Mode *returned_via; /* OPT */
	ILL_Amount *insured_for; /* OPT */
	ILL_String *requester_note; /* OPT */
	int num_returned_extensions;
	ILL_Extension **returned_extensions; /* OPT */
};

struct ILL_Checked_In {
#define ILL_Checked_In_version_1 1
#define ILL_Checked_In_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_ISO_Date *date_checked_in;
	ILL_String *responder_note; /* OPT */
	int num_checked_in_extensions;
	ILL_Extension **checked_in_extensions; /* OPT */
};

struct ILL_Overdue_ExtensionS {
	int num;
	ILL_Extension **elements;
};

struct ILL_Overdue {
#define ILL_Overdue_version_1 1
#define ILL_Overdue_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_Date_Due *date_due;
	ILL_String *responder_note; /* OPT */
	ILL_Overdue_ExtensionS *overdue_extensions; /* OPT */
};

struct ILL_Renew {
#define ILL_Renew_version_1 1
#define ILL_Renew_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_ISO_Date *desired_due_date; /* OPT */
	ILL_String *requester_note; /* OPT */
	int num_renew_extensions;
	ILL_Extension **renew_extensions; /* OPT */
};

struct ILL_Renew_Answer {
#define ILL_Renew_Answer_version_1 1
#define ILL_Renew_Answer_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	Odr_bool *answer;
	ILL_Date_Due *date_due; /* OPT */
	ILL_String *responder_note; /* OPT */
	int num_renew_answer_extensions;
	ILL_Extension **renew_answer_extensions; /* OPT */
};

struct ILL_Lost {
#define ILL_Lost_version_1 1
#define ILL_Lost_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_String *note; /* OPT */
	int num_lost_extensions;
	ILL_Extension **lost_extensions; /* OPT */
};

struct ILL_Damaged {
#define ILL_Damaged_version_1 1
#define ILL_Damaged_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_Damaged_Details *damaged_details; /* OPT */
	ILL_String *note; /* OPT */
	int num_damaged_extensions;
	ILL_Extension **damaged_extensions; /* OPT */
};

struct ILL_Message {
#define ILL_Message_version_1 1
#define ILL_Message_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_String *note;
	int num_message_extensions;
	ILL_Extension **message_extensions; /* OPT */
};

struct ILL_Status_Query {
#define ILL_Status_Query_version_1 1
#define ILL_Status_Query_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_String *note; /* OPT */
	int num_status_query_extensions;
	ILL_Extension **status_query_extensions; /* OPT */
};

struct ILL_Status_Or_Error_Report {
#define ILL_Status_Or_Error_Report_version_1 1
#define ILL_Status_Or_Error_Report_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	ILL_Reason_No_Report *reason_no_report; /* OPT */
	ILL_Status_Report *status_report; /* OPT */
	ILL_Error_Report *error_report; /* OPT */
	ILL_String *note; /* OPT */
	int num_status_or_error_report_extensions;
	ILL_Extension **status_or_error_report_extensions; /* OPT */
};

struct ILL_Expired {
#define ILL_Expired_version_1 1
#define ILL_Expired_version_2 2
	Odr_int *protocol_version_num;
	ILL_Transaction_Id *transaction_id;
	ILL_Service_Date_Time *service_date_time;
	ILL_System_Id *requester_id; /* OPT */
	ILL_System_Id *responder_id; /* OPT */
	int num_expired_extensions;
	ILL_Extension **expired_extensions; /* OPT */
};



struct ILL_Already_Forwarded {
	ILL_System_Id *responder_id;
	ILL_System_Address *responder_address; /* OPT */
};

struct ILL_Already_Tried_List_Type {
	int num;
	ILL_System_Id **elements;
};

struct ILL_Amount {
	char *currency_code; /* OPT */
	ILL_AmountString *monetary_value;
};



struct ILL_Client_Id {
	ILL_String *client_name; /* OPT */
	ILL_String *client_status; /* OPT */
	ILL_String *client_identifier; /* OPT */
};

struct ILL_Conditional_Results {
#define ILL_Conditional_Results_cost_exceeds_limit 13
#define ILL_Conditional_Results_charges 14
#define ILL_Conditional_Results_prepayment_required 15
#define ILL_Conditional_Results_lacks_copyright_compliance 16
#define ILL_Conditional_Results_library_use_only 22
#define ILL_Conditional_Results_no_reproduction 23
#define ILL_Conditional_Results_client_signature_required 24
#define ILL_Conditional_Results_special_collections_supervision_required 25
#define ILL_Conditional_Results_other 27
#define ILL_Conditional_Results_responder_specific 28
#define ILL_Conditional_Results_proposed_delivery_service 30
	Odr_int *conditions;
	ILL_ISO_Date *date_for_reply; /* OPT */
	int num_locations;
	ILL_Location_Info **locations; /* OPT */
	ILL_Delivery_Service *proposed_delivery_service; /* OPT */
};

struct ILL_Cost_Info_Type {
	ILL_Account_Number *account_number; /* OPT */
	ILL_Amount *maximum_cost; /* OPT */
	Odr_bool *reciprocal_agreement;
	Odr_bool *will_pay_fee;
	Odr_bool *payment_provided;
};

#define ILL_Current_State_nOT_SUPPLIED 1
#define ILL_Current_State_pENDING 2
#define ILL_Current_State_iN_PROCESS 3
#define ILL_Current_State_fORWARD 4
#define ILL_Current_State_cONDITIONAL 5
#define ILL_Current_State_cANCEL_PENDING 6
#define ILL_Current_State_cANCELLED 7
#define ILL_Current_State_sHIPPED 8
#define ILL_Current_State_rECEIVED 9
#define ILL_Current_State_rENEW_PENDING 10
#define ILL_Current_State_nOT_RECEIVED_OVERDUE 11
#define ILL_Current_State_rENEW_OVERDUE 12
#define ILL_Current_State_oVERDUE 13
#define ILL_Current_State_rETURNED 14
#define ILL_Current_State_cHECKED_IN 15
#define ILL_Current_State_rECALL 16
#define ILL_Current_State_lOST 17
#define ILL_Current_State_uNKNOWN 18

struct ILL_Damaged_DetailsSpecific_units {
	int num;
	Odr_int **elements;
};

struct ILL_Damaged_Details {
	Odr_oid *document_type_id; /* OPT */
	int which;
	union {
		Odr_null *complete_document;
		ILL_Damaged_DetailsSpecific_units *specific_units;
#define ILL_Damaged_Details_complete_document 1
#define ILL_Damaged_Details_specific_units 2
	} u;
};

struct ILL_Date_Due {
	ILL_ISO_Date *date_due_field;
	Odr_bool *renewable;
};

struct ILL_Delivery_Address {
	ILL_Postal_Address *postal_address; /* OPT */
	ILL_System_Address *electronic_address; /* OPT */
};

struct ILL_Delivery_ServiceElectronic_delivery {
	int num;
	ILL_Electronic_Delivery_Service **elements;
};

struct ILL_Delivery_Service {
	int which;
	union {
		ILL_Transportation_Mode *physical_delivery;
		ILL_Delivery_ServiceElectronic_delivery *electronic_delivery;
#define ILL_Delivery_Service_physical_delivery 1
#define ILL_Delivery_Service_electronic_delivery 2
	} u;
};

struct ILL_Electronic_Delivery_Service_0 {
	Odr_oid *e_delivery_mode;
	Odr_any *e_delivery_parameters;
};

struct ILL_Electronic_Delivery_Service_1 {
	Odr_oid *document_type_id;
	Odr_any *document_type_parameters;
};

struct ILL_Electronic_Delivery_Service {
	ILL_Electronic_Delivery_Service_0 *e_delivery_service; /* OPT */
	ILL_Electronic_Delivery_Service_1 *document_type; /* OPT */
	ILL_String *e_delivery_description; /* OPT */
	int which;
	union {
		ILL_System_Address *e_delivery_address;
		ILL_System_Id *e_delivery_id;
#define ILL_Electronic_Delivery_Service_e_delivery_address 1
#define ILL_Electronic_Delivery_Service_e_delivery_id 2
	} u;
	ILL_String *name_or_code; /* OPT */
	ILL_ISO_Time *delivery_time; /* OPT */
};

struct ILL_Error_Report {
	ILL_String *correlation_information;
	ILL_Report_Source *report_source;
	ILL_User_Error_Report *user_error_report; /* OPT */
	ILL_Provider_Error_Report *provider_error_report; /* OPT */
};

struct ILL_Estimate_Results {
	ILL_String *cost_estimate;
	int num_locations;
	ILL_Location_Info **locations; /* OPT */
};

struct ILL_Extension {
	Odr_int *identifier;
	Odr_bool *critical;
	Odr_any *item;
};

#define ILL_General_Problem_unrecognized_APDU 1
#define ILL_General_Problem_mistyped_APDU 2
#define ILL_General_Problem_badly_structured_APDU 3
#define ILL_General_Problem_protocol_version_not_supported 4
#define ILL_General_Problem_other 5

struct ILL_History_Report {
	ILL_ISO_Date *date_requested; /* OPT */
	ILL_String *author; /* OPT */
	ILL_String *title; /* OPT */
	ILL_String *author_of_article; /* OPT */
	ILL_String *title_of_article; /* OPT */
	ILL_ISO_Date *date_of_last_transition;
#define ILL_History_Report_iLL_REQUEST 1
#define ILL_History_Report_fORWARD 21
#define ILL_History_Report_fORWARD_NOTIFICATION 2
#define ILL_History_Report_sHIPPED 3
#define ILL_History_Report_iLL_ANSWER 4
#define ILL_History_Report_cONDITIONAL_REPLY 5
#define ILL_History_Report_cANCEL 6
#define ILL_History_Report_cANCEL_REPLY 7
#define ILL_History_Report_rECEIVED 8
#define ILL_History_Report_rECALL 9
#define ILL_History_Report_rETURNED 10
#define ILL_History_Report_cHECKED_IN 11
#define ILL_History_Report_rENEW_ANSWER 14
#define ILL_History_Report_lOST 15
#define ILL_History_Report_dAMAGED 16
#define ILL_History_Report_mESSAGE 17
#define ILL_History_Report_sTATUS_QUERY 18
#define ILL_History_Report_sTATUS_OR_ERROR_REPORT 19
#define ILL_History_Report_eXPIRED 20
	Odr_int *most_recent_service;
	ILL_ISO_Date *date_of_most_recent_service;
	ILL_System_Id *initiator_of_most_recent_service;
	ILL_Shipped_Service_Type *shipped_service_type; /* OPT */
	ILL_Transaction_Results *transaction_results; /* OPT */
	ILL_String *most_recent_service_note; /* OPT */
};

struct ILL_Hold_Placed_Results {
	ILL_ISO_Date *estimated_date_available;
	ILL_Medium_Type *hold_placed_medium_type; /* OPT */
	int num_locations;
	ILL_Location_Info **locations; /* OPT */
};

#define ILL_APDU_Type_iLL_REQUEST 1
#define ILL_APDU_Type_fORWARD_NOTIFICATION 2
#define ILL_APDU_Type_sHIPPED 3
#define ILL_APDU_Type_iLL_ANSWER 4
#define ILL_APDU_Type_cONDITIONAL_REPLY 5
#define ILL_APDU_Type_cANCEL 6
#define ILL_APDU_Type_cANCEL_REPLY 7
#define ILL_APDU_Type_rECEIVED 8
#define ILL_APDU_Type_rECALL 9
#define ILL_APDU_Type_rETURNED 10
#define ILL_APDU_Type_cHECKED_IN 11
#define ILL_APDU_Type_oVERDUE 12
#define ILL_APDU_Type_rENEW 13
#define ILL_APDU_Type_rENEW_ANSWER 14
#define ILL_APDU_Type_lOST 15
#define ILL_APDU_Type_dAMAGED 16
#define ILL_APDU_Type_mESSAGE 17
#define ILL_APDU_Type_sTATUS_QUERY 18
#define ILL_APDU_Type_sTATUS_OR_ERROR_REPORT 19
#define ILL_APDU_Type_eXPIRED 20

#define ILL_Service_Type_loan 1
#define ILL_Service_Type_copy_non_returnable 2
#define ILL_Service_Type_locations 3
#define ILL_Service_Type_estimate 4
#define ILL_Service_Type_responder_specific 5

struct ILL_String {
	int which;
	union {
		char *GeneralString;
		ILL_EDIFACTString *EDIFACTString;
#define ILL_String_GeneralString 1
#define ILL_String_EDIFACTString 2
	} u;
};

#define ILL_Intermediary_Problem_cannot_send_onward 1





struct ILL_Item_Id {
#define ILL_Item_Id_monograph 1
#define ILL_Item_Id_serial 2
#define ILL_Item_Id_other 3
	Odr_int *item_type; /* OPT */
	ILL_Medium_Type *held_medium_type; /* OPT */
	ILL_String *call_number; /* OPT */
	ILL_String *author; /* OPT */
	ILL_String *title; /* OPT */
	ILL_String *sub_title; /* OPT */
	ILL_String *sponsoring_body; /* OPT */
	ILL_String *place_of_publication; /* OPT */
	ILL_String *publisher; /* OPT */
	ILL_String *series_title_number; /* OPT */
	ILL_String *volume_issue; /* OPT */
	ILL_String *edition; /* OPT */
	ILL_String *publication_date; /* OPT */
	ILL_String *publication_date_of_component; /* OPT */
	ILL_String *author_of_article; /* OPT */
	ILL_String *title_of_article; /* OPT */
	ILL_String *pagination; /* OPT */
	Odr_external *national_bibliography_no; /* OPT */
	ILL_String *iSBN; /* OPT */
	ILL_String *iSSN; /* OPT */
	Odr_external *system_no; /* OPT */
	ILL_String *additional_no_letters; /* OPT */
	ILL_String *verification_reference_source; /* OPT */
};

struct ILL_Location_Info {
	ILL_System_Id *location_id;
	ILL_System_Address *location_address; /* OPT */
	ILL_String *location_note; /* OPT */
};

struct ILL_Locations_Results {
	ILL_Reason_Locs_Provided *reason_locs_provided; /* OPT */
	int num_locations;
	ILL_Location_Info **locations;
};

#define ILL_Medium_Type_printed 1
#define ILL_Medium_Type_microform 3
#define ILL_Medium_Type_film_or_video_recording 4
#define ILL_Medium_Type_audio_recording 5
#define ILL_Medium_Type_machine_readable 6
#define ILL_Medium_Type_other 7

struct ILL_Name_Of_Person_Or_Institution {
	int which;
	union {
		ILL_String *name_of_person;
		ILL_String *name_of_institution;
#define ILL_Name_Of_Person_Or_Institution_name_of_person 1
#define ILL_Name_Of_Person_Or_Institution_name_of_institution 2
	} u;
};

struct ILL_Person_Or_Institution_Symbol {
	int which;
	union {
		ILL_String *person_symbol;
		ILL_String *institution_symbol;
#define ILL_Person_Or_Institution_Symbol_person_symbol 1
#define ILL_Person_Or_Institution_Symbol_institution_symbol 2
	} u;
};

#define ILL_Place_On_Hold_Type_yes 1
#define ILL_Place_On_Hold_Type_no 2
#define ILL_Place_On_Hold_Type_according_to_responder_policy 3

struct ILL_Postal_Address {
	ILL_Name_Of_Person_Or_Institution *name_of_person_or_institution; /* OPT */
	ILL_String *extended_postal_delivery_address; /* OPT */
	ILL_String *street_and_number; /* OPT */
	ILL_String *post_office_box; /* OPT */
	ILL_String *city; /* OPT */
	ILL_String *region; /* OPT */
	ILL_String *country; /* OPT */
	ILL_String *postal_code; /* OPT */
};

struct ILL_Provider_Error_Report {
	int which;
	union {
		ILL_General_Problem *general_problem;
		ILL_Transaction_Id_Problem *transaction_id_problem;
		ILL_State_Transition_Prohibited *state_transition_prohibited;
#define ILL_Provider_Error_Report_general_problem 1
#define ILL_Provider_Error_Report_transaction_id_problem 2
#define ILL_Provider_Error_Report_state_transition_prohibited 3
	} u;
};

#define ILL_Reason_Locs_Provided_in_use_on_loan 1
#define ILL_Reason_Locs_Provided_in_process 2
#define ILL_Reason_Locs_Provided_lost 3
#define ILL_Reason_Locs_Provided_non_circulating 4
#define ILL_Reason_Locs_Provided_not_owned 5
#define ILL_Reason_Locs_Provided_on_order 6
#define ILL_Reason_Locs_Provided_volume_issue_not_yet_available 7
#define ILL_Reason_Locs_Provided_at_bindery 8
#define ILL_Reason_Locs_Provided_lacking 9
#define ILL_Reason_Locs_Provided_not_on_shelf 10
#define ILL_Reason_Locs_Provided_on_reserve 11
#define ILL_Reason_Locs_Provided_poor_condition 12
#define ILL_Reason_Locs_Provided_cost_exceeds_limit 13
#define ILL_Reason_Locs_Provided_on_hold 19
#define ILL_Reason_Locs_Provided_other 27
#define ILL_Reason_Locs_Provided_responder_specific 28

#define ILL_Reason_No_Report_temporary 1
#define ILL_Reason_No_Report_permanent 2

#define ILL_Reason_Unfilled_in_use_on_loan 1
#define ILL_Reason_Unfilled_in_process 2
#define ILL_Reason_Unfilled_lost 3
#define ILL_Reason_Unfilled_non_circulating 4
#define ILL_Reason_Unfilled_not_owned 5
#define ILL_Reason_Unfilled_on_order 6
#define ILL_Reason_Unfilled_volume_issue_not_yet_available 7
#define ILL_Reason_Unfilled_at_bindery 8
#define ILL_Reason_Unfilled_lacking 9
#define ILL_Reason_Unfilled_not_on_shelf 10
#define ILL_Reason_Unfilled_on_reserve 11
#define ILL_Reason_Unfilled_poor_condition 12
#define ILL_Reason_Unfilled_cost_exceeds_limit 13
#define ILL_Reason_Unfilled_charges 14
#define ILL_Reason_Unfilled_prepayment_required 15
#define ILL_Reason_Unfilled_lacks_copyright_compliance 16
#define ILL_Reason_Unfilled_not_found_as_cited 17
#define ILL_Reason_Unfilled_locations_not_found 18
#define ILL_Reason_Unfilled_on_hold 19
#define ILL_Reason_Unfilled_policy_problem 20
#define ILL_Reason_Unfilled_mandatory_messaging_not_supported 21
#define ILL_Reason_Unfilled_expiry_not_supported 22
#define ILL_Reason_Unfilled_requested_delivery_services_not_supported 23
#define ILL_Reason_Unfilled_preferred_delivery_time_not_possible 24
#define ILL_Reason_Unfilled_other 27
#define ILL_Reason_Unfilled_responder_specific 28

#define ILL_Report_Source_user 1
#define ILL_Report_Source_provider 2

struct ILL_Requester_Optional_Messages_Type {
	Odr_bool *can_send_RECEIVED;
	Odr_bool *can_send_RETURNED;
#define ILL_Requester_Optional_Messages_Type_requires 1
#define ILL_Requester_Optional_Messages_Type_desires 2
#define ILL_Requester_Optional_Messages_Type_neither 3
	Odr_int *requester_SHIPPED;
#define ILL_Requester_Optional_Messages_Type_requires 1
#define ILL_Requester_Optional_Messages_Type_desires 2
#define ILL_Requester_Optional_Messages_Type_neither 3
	Odr_int *requester_CHECKED_IN;
};

struct ILL_Responder_Optional_Messages_Type {
	Odr_bool *can_send_SHIPPED;
	Odr_bool *can_send_CHECKED_IN;
#define ILL_Responder_Optional_Messages_Type_requires 1
#define ILL_Responder_Optional_Messages_Type_desires 2
#define ILL_Responder_Optional_Messages_Type_neither 3
	Odr_int *responder_RECEIVED;
#define ILL_Responder_Optional_Messages_Type_requires 1
#define ILL_Responder_Optional_Messages_Type_desires 2
#define ILL_Responder_Optional_Messages_Type_neither 3
	Odr_int *responder_RETURNED;
};

struct ILL_Retry_Results {
#define ILL_Retry_Results_in_use_on_loan 1
#define ILL_Retry_Results_in_process 2
#define ILL_Retry_Results_on_order 6
#define ILL_Retry_Results_volume_issue_not_yet_available 7
#define ILL_Retry_Results_at_bindery 8
#define ILL_Retry_Results_cost_exceeds_limit 13
#define ILL_Retry_Results_charges 14
#define ILL_Retry_Results_prepayment_required 15
#define ILL_Retry_Results_lacks_copyright_compliance 16
#define ILL_Retry_Results_not_found_as_cited 17
#define ILL_Retry_Results_on_hold 19
#define ILL_Retry_Results_other 27
#define ILL_Retry_Results_responder_specific 28
	Odr_int *reason_not_available; /* OPT */
	ILL_ISO_Date *retry_date; /* OPT */
	int num_locations;
	ILL_Location_Info **locations; /* OPT */
};

struct ILL_Search_Type {
	ILL_String *level_of_service; /* OPT */
	ILL_ISO_Date *need_before_date; /* OPT */
#define ILL_Search_Type_need_Before_Date 1
#define ILL_Search_Type_other_Date 2
#define ILL_Search_Type_no_Expiry 3
	Odr_int *expiry_flag;
	ILL_ISO_Date *expiry_date; /* OPT */
};



struct ILL_Send_To_List_Type_s {
	ILL_System_Id *system_id;
	ILL_Account_Number *account_number; /* OPT */
	ILL_System_Address *system_address; /* OPT */
};

struct ILL_Send_To_List_Type {
	int num;
	ILL_Send_To_List_Type_s **elements;
};

struct ILL_Service_Date_this {
	ILL_ISO_Date *date;
	ILL_ISO_Time *time; /* OPT */
};

struct ILL_Service_Date_original {
	ILL_ISO_Date *date;
	ILL_ISO_Time *time; /* OPT */
};

struct ILL_Service_Date_Time {
	ILL_Service_Date_this *date_time_of_this_service;
	ILL_Service_Date_original *date_time_of_original_service; /* OPT */
};



struct ILL_State_Transition_Prohibited {
	ILL_APDU_Type *aPDU_type;
	ILL_Current_State *current_state;
};

struct ILL_Status_Report {
	ILL_History_Report *user_status_report;
	ILL_Current_State *provider_status_report;
};

struct ILL_Supplemental_Item_Description {
	int num;
	Odr_external **elements;
};

struct ILL_Supply_Details {
	ILL_ISO_Date *date_shipped; /* OPT */
	ILL_Date_Due *date_due; /* OPT */
	Odr_int *chargeable_units; /* OPT */
	ILL_Amount *cost; /* OPT */
#define ILL_Supply_Details_library_use_only 22
#define ILL_Supply_Details_no_reproduction 23
#define ILL_Supply_Details_client_signature_required 24
#define ILL_Supply_Details_special_collections_supervision_required 25
#define ILL_Supply_Details_other 27
	Odr_int *shipped_conditions; /* OPT */
	int which;
	union {
		ILL_Transportation_Mode *physical_delivery;
		ILL_Electronic_Delivery_Service *electronic_delivery;
#define ILL_Supply_Details_physical_delivery 1
#define ILL_Supply_Details_electronic_delivery 2
	} u; /* OPT */
	ILL_Amount *insured_for; /* OPT */
	ILL_Amount *return_insurance_require; /* OPT */
	int num_no_of_units_per_medium;
	ILL_Units_Per_Medium_Type **no_of_units_per_medium; /* OPT */
};

struct ILL_Supply_Medium_Info_Type {
	ILL_Supply_Medium_Type *supply_medium_type;
	ILL_String *medium_characteristics; /* OPT */
};

#define ILL_Supply_Medium_Type_printed 1
#define ILL_Supply_Medium_Type_photocopy 2
#define ILL_Supply_Medium_Type_microform 3
#define ILL_Supply_Medium_Type_film_or_video_recording 4
#define ILL_Supply_Medium_Type_audio_recording 5
#define ILL_Supply_Medium_Type_machine_readable 6
#define ILL_Supply_Medium_Type_other 7

struct ILL_System_Address {
	ILL_String *telecom_service_identifier; /* OPT */
	ILL_String *telecom_service_address; /* OPT */
};

struct ILL_System_Id {
	ILL_Person_Or_Institution_Symbol *person_or_institution_symbol; /* OPT */
	ILL_Name_Of_Person_Or_Institution *name_of_person_or_institution; /* OPT */
};

struct ILL_Third_Party_Info_Type {
	Odr_bool *permission_to_forward;
	Odr_bool *permission_to_chain;
	Odr_bool *permission_to_partition;
	Odr_bool *permission_to_change_send_to_list;
	ILL_System_Address *initial_requester_address; /* OPT */
#define ILL_Third_Party_Info_Type_ordered 1
#define ILL_Third_Party_Info_Type_unordered 2
	Odr_int *preference;
	ILL_Send_To_List_Type *send_to_list; /* OPT */
	ILL_Already_Tried_List_Type *already_tried_list; /* OPT */
};

struct ILL_Transaction_Id {
	ILL_System_Id *initial_requester_id; /* OPT */
	ILL_String *transaction_group_qualifier;
	ILL_String *transaction_qualifier;
	ILL_String *sub_transaction_qualifier; /* OPT */
};

#define ILL_Transaction_Id_Problem_duplicate_transaction_id 1
#define ILL_Transaction_Id_Problem_invalid_transaction_id 2
#define ILL_Transaction_Id_Problem_unknown_transaction_id 3

#define ILL_Transaction_Results_conditional 1
#define ILL_Transaction_Results_retry 2
#define ILL_Transaction_Results_unfilled 3
#define ILL_Transaction_Results_locations_provided 4
#define ILL_Transaction_Results_will_supply 5
#define ILL_Transaction_Results_hold_placed 6
#define ILL_Transaction_Results_estimate 7

#define ILL_Transaction_Type_simple 1
#define ILL_Transaction_Type_chained 2
#define ILL_Transaction_Type_partitioned 3



#define ILL_Unable_To_Perform_not_available 1
#define ILL_Unable_To_Perform_resource_limitation 2
#define ILL_Unable_To_Perform_other 3

struct ILL_Unfilled_Results {
	ILL_Reason_Unfilled *reason_unfilled;
	int num_locations;
	ILL_Location_Info **locations; /* OPT */
};

struct ILL_Units_Per_Medium_Type {
	ILL_Supply_Medium_Type *medium;
	Odr_int *no_of_units;
};

struct ILL_User_Error_Report {
	int which;
	union {
		ILL_Already_Forwarded *already_forwarded;
		ILL_Intermediary_Problem *intermediary_problem;
		ILL_Security_Problem *security_problem;
		ILL_Unable_To_Perform *unable_to_perform;
#define ILL_User_Error_Report_already_forwarded 1
#define ILL_User_Error_Report_intermediary_problem 2
#define ILL_User_Error_Report_security_problem 3
#define ILL_User_Error_Report_unable_to_perform 4
	} u;
};

struct ILL_Will_Supply_Results {
#define ILL_Will_Supply_Results_in_use_on_loan 1
#define ILL_Will_Supply_Results_in_process 2
#define ILL_Will_Supply_Results_on_order 6
#define ILL_Will_Supply_Results_at_bindery 8
#define ILL_Will_Supply_Results_on_hold 19
#define ILL_Will_Supply_Results_being_processed_for_supply 26
#define ILL_Will_Supply_Results_other 27
#define ILL_Will_Supply_Results_responder_specific 28
#define ILL_Will_Supply_Results_electronic_delivery 30
	Odr_int *reason_will_supply;
	ILL_ISO_Date *supply_date; /* OPT */
	ILL_Postal_Address *return_to_address; /* OPT */
	int num_locations;
	ILL_Location_Info **locations; /* OPT */
	ILL_Electronic_Delivery_Service *electronic_delivery_service; /* OPT */
};



#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
