#ifndef __AUTH_TYPES_H__
#define __AUTH_TYPES_H__

/// PAS AUTH Result Value Definition
//#define AUTH_RESULT_TIME_OUT		0x00000021		///< PAS_AUTH Timeout Result
//#define AUTH_RESULT_ANONYMOUS_CONN  0x00000006		///< PAS_AUTH Anonymous Result
//#define AUTH_RESULT_IPLS_NOT_CHECK  0x00000016		///< IPLS ¹Ì Ã¼Å© Result
//#define AUTH_RESULT_IP_ADDR_NULL    0x00000017		///< No Search NOT Search
//#define AUTH_RESULT_NOT_MDN         0x00000018		///< NOT MDN

#define PAS_AUTH_FAIL_NOT_ADDR          0x00000001
#define PAS_AUTH_FAIL_AT_NOT_FOUND      0x00000006
#define PAS_AUTH_FAIL_MSISDN            0x0000000A
#define PAS_AUTH_FAIL_IPADDR            0x0000000B
#define PAS_AUTH_FAIL_INVALID_IP_A_CLASS    0x00000013


namespace AUTH
{
	const int MIN_NUM_LEN = 16;
	const int REQUEST_AUTHORIZE_CODE = 0x00000001;

	struct RequestBody
	{
		int type;
		int seq;
		char min[MIN_NUM_LEN];
		unsigned int accessIP;
		int port;
		int startConn;
		int newBrowser;
		int g3GCode;
		char mdn[MIN_NUM_LEN];
		char msModel[MIN_NUM_LEN];
	};

	struct ResponseBody
	{
		int type;
		int status;
		char ackMin[16];
	};
}

#endif
