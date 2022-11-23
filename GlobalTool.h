#ifndef GLOBAL_TOOL_H__20221123
#define GLOBAL_TOOL_H__20221123

#include <vector>
#include <string>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <ctime>
#include <set>


#pragma pack(1)//强制内存对齐,系统不在填补


typedef enum micro_data_type {

	OMRON_BOOL_TYPE = 0xc1, 
	OMRON_SINT_TYPE = 0xc2,
	OMRON_INT_TYPE = 0xc3,
	OMRON_DINT_TYPE = 0xc4,
	OMRON_LINT_TYPE = 0xc5,
	OMRON_USINT_TYPE = 0xc6,
	OMRON_UINT_TYPE = 0xc7,
	OMRON_UDINT_TYPE = 0xc8,
	OMRON_ULINT_TYPE = 0xc9,
	OMRON_REAL_TYPE = 0xca,
	OMRON_LREAL_TYPE = 0xcb,
	OMRON_BYTE_TYPE = 0xc6,
	OMRON_WORD_TYPE = 0xC7,
	OMRON_DWORD_TYPE = 0xc8,
	OMRON_LWORD_TYPE = 0xd4,
	//OMRON_ENUM_TYPE = 0x07,
	OMRON_STRING_TYPE = 0xda,
}MICRO_DATA_TYPE;



typedef enum eip_request_cmd {

	/* RegisterSession commands */
	EIP_REGISTER_CMD = 0x65,
	/* SendRRData commands */
	EIP_SENDRRDATA_CMD = 0x6f,
	/* SendUnitData commands */
	EIP_SENDUNITDATA_CMD = 0x70,
}EIP_REQUEST_CMD;

typedef enum cip_service_code {

	CIP_FORWARD_CLOSE_CODE = 0x4e,
	CIP_LARGE_FORWARD_OPEN_CODE = 0x5b,
	CIP_FORWARD_OPEN_CODE = 0x54,
	CIP_READ_CODE = 0x4c,
	CIP_WRITE_CODE = 0x4d,
}CIP_SERVICE_CODE;

typedef enum cip_request_types {

	CIP_READ_REQUEST_TYPE,
	CIP_WRITE_REQUEST_TYPE,
}CIP_REQUEST_TYPES;

typedef enum cip_response_types {

	CIP_READ_RESPONSE_TYPE,
	CIP_WRITE_RESPONSE_TYPE,
}CIP_RESPONSE_TYPES;


/*
*Defined in EtherNet-IP(Chinese)_Edition1.2.pdf P18:2-3.1
*Encapsulation header
*Fixed size is 24 bytes
**/
typedef struct tagEIPHEADER_T {

	uint16_t usEIPCommand;		/* 2 bytes, Encapsulate commands */
	uint16_t usDataLength;		/* 2 bytes, Length of data part */
	uint32_t unSessionHandle;	/* 4 bytes, conversation handle */
	uint32_t unStatus;			/* 4 bytes, Status code, valid as 0 */
	uint64_t ullSenderContext;	/* 8 bytes, Sender Context */
	uint32_t unOptions;			/* 4 bytes, Option, valid is 0 */
}EIPHEADER_T;

/*
*Defined in EtherNet-IP(Chinese)_Edition1.2.pdf P33:2-4.8
*Encapsulation data when there is no connection
*Fixed size is 16 bytes
**/
typedef struct tagUNCONNECT_EIPDATA_T {

	uint32_t unInterfaceHandl;		/* 4 bytes, Interface handle, CIP pattern is 0*/
	uint16_t usTimeOut;				/* 2 bytes, Timeout setting, 0 uses the default */

	/* P36:2-6.1 */
	uint16_t usItemCounts;			/* 2 bytes, item count, Usually an address item and a data item */
	uint16_t usAddressItemTypeID;	/* 2 bytes, address type id is 0 when there is no connection */
	uint16_t usAddressItemLength;	/* 2 bytes, address length is 0 when there is no connection */
	uint16_t usDataItemTypeID;		/* 2 bytes, data type id is 0xB2 when there is no connection */
	uint16_t usCIPMessageLength;	/* 2 bytes, CIP message length when there is no connection */
}UNCONNECT_EIPDATA_T;

/*
* Defined in EtherNet-IP(Chinese)_Edition1.2.pdf P33:2-4.8
* Encapsulation data when connected
* Fixed size is 22 bytes
* The definition here is not universal,  only for read and write operations
**/
typedef struct tagCONNECT_EIPDATA_T {

	uint32_t unInterfaceHandl;		/* 4 bytes, Interface handle, CIP pattern is 0*/
	uint16_t usTimeOut;				/* 2 bytes, Timeout setting, 0 uses the default */

	/* EtherNet-IP(Chinese)_Edition1.2.pdf P36:2-6.1 */
	uint16_t usItemCounts;			/* 2 bytes, item count, Usually an address item and a data item */
	uint16_t usAddressItemTypeID;	/* 2 bytes, address type id is 0xA1 when connected */
	uint16_t usAddressItemLength;	/* 2 bytes, address length is 4 when connected */
	uint32_t unConectionID;			/* 4 bytes, connect id */
	uint16_t usDataItemTypeID;		/* 2 bytes, data type id is 0xB1 when connected */
	uint16_t usCIPMessageLength;	/* 2 bytes, CIP message length when connected */
	uint16_t usCIPSquenceCount;		/* 2 bytes, CIP Serial Number */
}CONNECT_EIPDATA_T;


/*
* use in LARGE FORWARD OPEN REQUEST and FORWARD CLOSE REQUEST
* Put at the end of the message
* The relevant definitions are in CommonIndustrialProtocol-V1-3.3_2007.pdf P1228 C-1.4
**/
typedef struct tagCONNECTIONPATH_T {

	/* port */
	//uint8_t ucPathSegment1;	/* 1 bytes */
	//uint8_t ucLinkAddress;	/* 1 bytes */
	/* class */
	uint8_t ucPathSegment2;	/* 1 bytes */
	uint8_t ucClass;		/* 1 bytes */
	/* istance */
	uint8_t ucPathSegment3;	/* 1 bytes */
	uint8_t ucIstance;		/* 1 bytes */
}CONNECTIONPATH_T;


typedef struct tagUNCONNECT_COMMOMINDUSTRIALPROTOCOL_T {

	uint8_t ucServiceCode;
	uint8_t ucPathSize;
	/* class */
	uint8_t ucPathSegment1;	/* 1 bytes */
	uint8_t ucClass;		/* 1 bytes */
	/* istance */
	uint8_t ucPathSegment2;	/* 1 bytes */
	uint8_t ucIstance;		/* 1 bytes */
}UNCONNECT_COMMOMINDUSTRIALPROTOCOL_T;



/*
* use in FORWARD CLOSE REQUEST
* CLOASE REQUSET = CONNECT_EIPDATA_T + UNCONNECT_EIPDATA_T + UNCONNECT_COMMOMINDUSTRIALPROTOCOL_T + CLOSE_CIPCONNECTIOPNMANAGER_T
**/
typedef struct tagCLOSE_CIPCONNECTIOPNMANAGER_T {

	/*
	*2bytes
	*first bytes is tick time
	*second bytes is time-out tick
	*ActualTimeOut = 2^(tick time) * time-out tick
	**/
	uint16_t usActualTimeOut;
	/* 2bytes, the last two bits of the local IP  */
	uint16_t usConnectionSerialNumber;
	/* 2bytes, fixed value is 0x30 */
	uint16_t usOriginatorVendorID;
	/* 4 bytes, the meaning of the last two bytes is unknown, the first two bytes are the first two bits of the IP */
	uint32_t unOriginatorSerialNumber;
	/* 1 bytes, the count of connectionPath */
	uint8_t ucConnectionPathSize;
	/* 1 bytes, reserved field, usually 0 */
	uint8_t ucReserved;
	/* 2 bytes is port, 2 bytes is class, 2 bytes is istance */
	CONNECTIONPATH_T connectionPath;
}CLOSE_CIPCONNECTIOPNMANAGER_T;


/*
* OPEN REQUEST = CONNECT_EIPDATA_T + UNCONNECT_EIPDATA_T + UNCONNECT_COMMOMINDUSTRIALPROTOCOL_T + OPEN_CIPCONNECTIOPNMANAGER_T
*/
typedef struct tagLarge_Forward_Open_CIPCONNECTIOPNMANAGER_T {

	/*
	*2bytes
	*first bytes is tick time
	*second bytes is time-out tick
	*ActualTimeOut = 2^(tick time) * time-out tick
	**/
	uint16_t usActualTimeOut;

	/*
	* O->T T->O : in CommonIndustrialProtocol-V1-3.3_2007.pdf	P28 1-8
	* O->T: Originator to Target (used to describe packets that are sent from the originator to the target)
	* T->O: Target to Originator (used to describe packets that are sent from the target to the originator)
	*/

	/* 4 bytes, O->T connection identifier */
	uint32_t unOTNetworkConnectionID;
	/* 4 bytes, T->O connection identifier */
	uint32_t unTONetworkConnectionID;
	/* 2bytes, the last two bits of the local IP  */
	uint16_t usConnectionSerialNumber;
	/* 2bytes, fixed value is 0x30 */
	uint16_t usOriginatorVendorID;
	/* 4 bytes, the meaning of the last two bytes is unknown, the first two bytes are the first two bits of the IP */
	uint32_t unOriginatorSerialNumber;

	/*
	* 1 byte is Connection Time Out Multiplier, 3 bytes is Reserved
	* Connection Time Out Multiplier: Defined in CommonIndustrialProtocol-V1-3.3_2007.pdf P99 3-5.5.1.5
	* Reserved: reserved field, usually 0x000000
	*/
	uint32_t unConnectionTimeOutMulAndReserved;

	/* Network Connection Parameters: Defined in CommonIndustrialProtocol-V1-3.3_2007.pdf P95 3-5.5.1.1 */
	/* RPI: Defined in CommonIndustrialProtocol-V1-3.3_2007.pdf P104 3-5.5.2 Forward Open and Large_Forward_Open */

	uint32_t unOTRPI;
	uint32_t unOTNetworkConnectionParam;
	uint32_t unTORPI;
	uint32_t unTONetworkConnectionParam;

	/*
	* 1 bytes
	* Defined in CommonIndustrialProtocol-V1-3.3_2007.pdf P53 3-4.4.3
	* The eighth is  Direction bit, 0: the target end is the server; 1: the target end is the client
	* The fourth to seventh places are the value of Production Trigger:
	*		0: Cyclic
	*		1: Change Of State
	*		2: Application Object Triggered
	*		other: Reserved by CIP
	* The first to fourth places are the transportation category
	*/
	uint8_t ucTrigger;

	/* 1 bytes, count of connectionPath */
	uint8_t ucConnectionPathSize;
	CONNECTIONPATH_T connectionPath;
}Large_Forward_Open_CIPCONNECTIOPNMANAGER_T;

typedef struct tagForward_Open_CIPCONNECTIOPNMANAGER_T {

	/*
	*2bytes
	*first bytes is tick time
	*second bytes is time-out tick
	*ActualTimeOut = 2^(tick time) * time-out tick
	**/
	uint16_t usActualTimeOut;

	/*
	* O->T T->O : in CommonIndustrialProtocol-V1-3.3_2007.pdf	P28 1-8
	* O->T: Originator to Target (used to describe packets that are sent from the originator to the target)
	* T->O: Target to Originator (used to describe packets that are sent from the target to the originator)
	*/

	/* 4 bytes, O->T connection identifier */
	uint32_t unOTNetworkConnectionID;
	/* 4 bytes, T->O connection identifier */
	uint32_t unTONetworkConnectionID;
	/* 2bytes, the last two bits of the local IP  */
	uint16_t usConnectionSerialNumber;
	/* 2bytes, fixed value is 0x30 */
	uint16_t usOriginatorVendorID;
	/* 4 bytes, the meaning of the last two bytes is unknown, the first two bytes are the first two bits of the IP */
	uint32_t unOriginatorSerialNumber;

	/*
	* 1 byte is Connection Time Out Multiplier, 3 bytes is Reserved
	* Connection Time Out Multiplier: Defined in CommonIndustrialProtocol-V1-3.3_2007.pdf P99 3-5.5.1.5
	* Reserved: reserved field, usually 0x000000
	*/
	uint32_t unConnectionTimeOutMulAndReserved;

	/* Network Connection Parameters: Defined in CommonIndustrialProtocol-V1-3.3_2007.pdf P95 3-5.5.1.1 */
	/* RPI: Defined in CommonIndustrialProtocol-V1-3.3_2007.pdf P104 3-5.5.2 Forward Open and Large_Forward_Open */

	uint32_t unOTRPI;
	uint16_t unOTNetworkConnectionParam;
	uint32_t unTORPI;
	uint16_t unTONetworkConnectionParam;

	/*
	* 1 bytes
	* Defined in CommonIndustrialProtocol-V1-3.3_2007.pdf P53 3-4.4.3
	* The eighth is  Direction bit, 0: the target end is the server; 1: the target end is the client
	* The fourth to seventh places are the value of Production Trigger:
	*		0: Cyclic
	*		1: Change Of State
	*		2: Application Object Triggered
	*		other: Reserved by CIP
	* The first to fourth places are the transportation category
	*/
	uint8_t ucTrigger;

	/* 1 bytes, count of connectionPath */
	uint8_t ucConnectionPathSize;
	CONNECTIONPATH_T connectionPath;
}Forward_Open_CIPCONNECTIOPNMANAGER_T;


typedef struct tagREAD_COMMOMINDUSTRIALPROTOCOL_T {

	uint8_t ucServiceCode;
	uint8_t ucPathSize;
	uint8_t ucPathSegmentType;
	uint8_t ucDataSize;
	std::string strANSISymbol;
}READ_COMMOMINDUSTRIALPROTOCOL_T;


#pragma pack()  /* close */ 


namespace Tool {

	static std::set<uint16_t> setSerialNumber;

	void initRand();

	uint32_t getRand();

	uint16_t getConnectionSerialNumber();

};

#endif /* GLOBAL_TOOL_H__20221123 */