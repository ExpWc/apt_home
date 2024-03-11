/*
 * DataType.h
 *
 *  Created on: 2012-9-20
 *      Author: zs
 */
#ifndef DATATYPE_H_
#define DATATYPE_H_

#define MAX_AFDX_PAYLOAD_LEN 1530
#define MAX_429_PAYLOAD_LEN 4
#define MAX_NOBUS_PAYLOAD_LEN 8
#define MAX_CAN_PAYLOAD_LEN 8
#define MAX_717_PAYLOAD_LEN 2048
#define MAX_708_PAYLOAD_LEN 200


//add by ty
#define MAX_DIO_PAYLOAD_LEN 8
#define MAX_ANALOG_PAYLOAD_LEN 8

//added by ping.song
#define MAX_RFM_PAYLOAD_LEN 2048
#define MAX_422_PAYLOAD_LEN 2048
#define MAX_HIL_PAYLOAD_LEN 2048
#define MAX_SIL_PAYLOAD_LEN 2048
#define MAX_HDLC_PAYLOAD_LEN 2048
#define MAX_ETH_PAYLOAD_LEN 1518
#define MAX_1553_PAYLOAD_LEN 64
#define MAX_IMBADB_PAYLOAD_LEN 2048
#define MAX_FLEXRAY_PAYLOAD_LEN 127
#define MAX_1394_PAYLOAD_LEN 2048
#ifdef _WIN32
#define EX_PAYLOAD 2000
#else
#define EX_PAYLOAD 0
#endif
#define PAYLOAD_LEN 4096
#define MAX_DATA_LEN (PAYLOAD_LvalueEN+5*4)

#pragma pack(push,1)

typedef struct _storage_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int src;
	unsigned int dataType;
	unsigned int length;
	unsigned int sec;
	unsigned int usec;
	char payload[PAYLOAD_LEN];
} TY_STORAGE_DATA;

typedef struct _storage_model_data
{
	char fullPath[128];
	unsigned int length;
	unsigned int sec;
	unsigned int usec;
	char payload[8];
} TY_STORAGE_MODEL_DATA;

typedef struct _storage_analog_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int length;
	unsigned int sec;
	unsigned int usec;
	char payload[EX_PAYLOAD];
} TY_STORAGE_ANALOG_DATA;

typedef struct _storage_can_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int type;
	unsigned int can_id;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_CAN_DATA;

typedef struct _storage_429_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned short label;
	unsigned int sec;
	unsigned int usec;
	unsigned int length; //length of payload
	char payload[EX_PAYLOAD];
} TY_STORAGE_429_DATA;

typedef struct _storage_717_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int sec;
	unsigned int usec;
	unsigned int length; //length of payload
	char payload[EX_PAYLOAD];
} TY_STORAGE_717_DATA;

typedef struct _storage_708_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned short label;
	unsigned int sec;
	unsigned int usec;
	unsigned int length; //length of payload
	char payload[EX_PAYLOAD];
} TY_STORAGE_708_DATA;

typedef struct _storage_nobus_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int sec;
	unsigned int usec;
	unsigned int length; //length of payload
	char payload[EX_PAYLOAD];
} TY_STORAGE_NOBUS_DATA;

/* deleted by ping.song for DDS1.4.2 or above */
typedef struct _afdx_para
{
	unsigned char src_mac[6];
	unsigned char dst_mac[6];
	unsigned int src_ip;
	unsigned int dst_ip;
	unsigned short src_port;
	unsigned short dst_port;
	unsigned int vl;
} AFDX_PARA;
/* modified by ping.song for DDS1.4.2 or above */
typedef struct _storage_afdx_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int vl;
	unsigned int comPort;
	unsigned int net_flag;
	//  AFDX_PARA afdxSrc;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	unsigned int headerLength;
	unsigned int crcLength;
	char payload[EX_PAYLOAD];
} TY_STORAGE_AFDX_DATA;

//added by ping.song
typedef struct _storage_rfm_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int addr;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_RFM_DATA;
typedef struct _storage_422_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	char frame_begin[4];
	unsigned int frame_begin_length;
	char frame_end[4];
	unsigned int frame_end_length;
	unsigned int reserved;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_422_DATA;

typedef struct _storage_hil_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_HIL_DATA;

typedef struct _storage_sil_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_SIL_DATA;

typedef struct hdlc_para
{
	char slave_addr;
	char type;
	char control_order;
} HDLC_PARA;

typedef struct _storage_hdlc_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int block_id;
	unsigned int offset;
	unsigned int reserved;
	HDLC_PARA parse;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_HDLC_DATA;

typedef struct _storage_eth_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned char src_mac[6];
	unsigned char dst_mac[6];
	unsigned short protocol;//for dds1.4.5
	unsigned int src_ip;
	unsigned int dst_ip;
	unsigned short src_port;
	unsigned short dst_port;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_ETH_DATA;
typedef struct
{
	unsigned short tf; // terminal flag bit (LSB)
	unsigned short dba; // dynamic bus acceptance flag bit
	unsigned short sf; // subsystem flag bit
	unsigned short busy; // busy flag bit
	unsigned short bcr; // broadcast received bit
	unsigned short res; // unused bits
	unsigned short sr; // service request
	unsigned short inst; // instrumentation bit
	unsigned short me; // message error
	unsigned short rtaddr; // rt address field (MSB)
} B1553_STATUS;

typedef struct _storage_1553_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int mode;
	unsigned int src_addr;
	unsigned int sub_src_addr;
	unsigned int dst_addr;
	unsigned int sub_dst_addr;
	B1553_STATUS status1;
	B1553_STATUS status2;
	unsigned int src;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_1553_DATA;
typedef struct _storage_imb_adb_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int type;
	unsigned int label;
	unsigned int err;
	unsigned int crc;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_IMB_ADB_DATA;
typedef struct _storage_1394_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int msgId;
	unsigned int nodeId;
	unsigned int channelId;
	unsigned int heartBeart;
	unsigned int healthStatus;
	bool VPCError;
	unsigned int sec;
	unsigned int usec;
	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_1394_DATA;

typedef struct _storage_flexray_data
{
	char uuid[128];
	unsigned int cardId;
	unsigned int chnId;
	unsigned int sec;
	unsigned int usec;

	unsigned int PPI;
	unsigned int NUF;
	unsigned int SYF;
	unsigned int SUF;
	unsigned int frameId;
	unsigned int cyccnt;
	unsigned int headerCRC;
	unsigned int chAorBrecv;
	unsigned int syntaxError;
	unsigned int contentError;
	unsigned int violationError;
	unsigned int chipReadError;

	unsigned int length;
	char payload[EX_PAYLOAD];
} TY_STORAGE_FLEXRAY_DATA;

#pragma pack(pop)

#endif /* DATATYPE_H_ */
