#ifndef __SYSTEM_STATISTIC_H__
#define __SYSTEM_STATISTIC_H__

/**
@brief PASGW 시스템 통계 정보 저장
*/


#include "Common.h"

class SystemStatistic
{
// 멤버함수
public:
	SystemStatistic(void);
	~SystemStatistic(void);

	static SystemStatistic* instance();

	// Client Statistic Function
	size_t clientConnectionAccept();
	void clientConnectionAccept(int size);

	size_t clientCloseByHost();
	void clientCloseByHost(int size);

	size_t clientCloseByPeer();
	void clientCloseByPeer(int size);

	size_t clientRequest();
	void clientRequest(int size);

	size_t clientUpload();
	void clientUpload(int size);

	size_t clientResponse();
	void clientResponse(int size);

	// CP Statistic Function
	size_t cpConnectionRequest();
	void cpConnectionRequest(int size);

	size_t cpConnectionEstablished();
	void cpConnectionEstablished(int size);

	size_t cpCloseByHost();
	void cpCloseByHost(int size);

	size_t cpCloseByPeer();
	void cpCloseByPeer(int size);

	size_t cpRequest();
	void cpRequest(int size);

	size_t cpUpload();
	void cpUpload(int size);

	size_t cpResponse();
	void cpResponse(int size);

	size_t cpStream();
	void cpStream(int size);

	size_t cpChunked();
	void cpChunked(int size);

	size_t cpOdd();
	void cpOdd(int size);

	size_t cpNormal();
	void cpNormal(int size);

	void writeLog();

// 멤버변수
private:
	// Client Statistic
	size_t countClientConnectionAccept;
	size_t countClientCloseByHost;
	size_t countClientCloseByPeer;
	size_t countClientRequest;
	size_t countClientUpload;
	size_t countClientResponse;

	// CP Statistic
	size_t countCPConnectionRequest;
	size_t countCPConnectionEstablished;
	size_t countCPCloseByHost;
	size_t countCPCloseByPeer;
	size_t countCPRequest;
	size_t countCPUpload;
	size_t countCPResponse;
	
	size_t countCPNormal; ///< Content-Length 가 있는 일반 데이터
	size_t countCPOdd; ///< Content-Length 가 없는 일반 데이터
	size_t countCPChunked; ///< Chunk 형식 데이터
	size_t countCPStream; ///< 스트리밍으로 전송한 데이터
	
};

#endif

