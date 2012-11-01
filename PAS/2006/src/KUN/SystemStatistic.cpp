#include "SystemStatistic.h"
#include "Mutex.h"
#include "PasLog.h"

SystemStatistic::SystemStatistic(void)
{
	// Client Statistic
	countClientConnectionAccept = 0;
	countClientCloseByHost = 0;
	countClientCloseByPeer = 0;
	countClientRequest = 0;
	countClientUpload = 0;
	countClientResponse = 0;

	// CP Statistic
	countCPConnectionRequest = 0;
	countCPConnectionEstablished = 0;
	countCPCloseByHost = 0;
	countCPCloseByPeer = 0;
	countCPRequest = 0;
	countCPUpload = 0;
	countCPResponse = 0;
	
	countCPNormal = 0;
	countCPChunked = 0;
	countCPOdd = 0;
	countCPStream = 0;
}

SystemStatistic::~SystemStatistic(void)
{
}

SystemStatistic* SystemStatistic::instance()
{
	return ACE_Singleton<SystemStatistic, PAS_SYNCH_MUTEX>::instance();
}

size_t SystemStatistic::clientConnectionAccept()
{
	return countClientConnectionAccept;
}

void SystemStatistic::clientConnectionAccept(int size)
{
	countClientConnectionAccept += size;
}

size_t SystemStatistic::clientCloseByHost()
{
	return countClientCloseByHost;
}

void SystemStatistic::clientCloseByHost(int size)
{
	countClientCloseByHost += size;
}

size_t SystemStatistic::clientCloseByPeer()
{
	return countClientCloseByPeer;
}

void SystemStatistic::clientCloseByPeer(int size)
{
	countClientCloseByPeer += size;
}

size_t SystemStatistic::clientRequest()
{
	return countClientRequest;
}

void SystemStatistic::clientRequest(int size)
{
	countClientRequest += size;
}

size_t SystemStatistic::clientUpload()
{
	return countClientUpload;
}

void SystemStatistic::clientUpload(int size)
{
	countClientUpload += size;
}

size_t SystemStatistic::clientResponse()
{
	return countClientResponse;
}

void SystemStatistic::clientResponse(int size)
{
	countClientResponse += size;
}

size_t SystemStatistic::cpConnectionRequest()
{
	return countCPConnectionRequest;
}

void SystemStatistic::cpConnectionRequest(int size)
{
	countCPConnectionRequest += size;
}

size_t SystemStatistic::cpConnectionEstablished()
{
	return countCPConnectionEstablished;
}

void SystemStatistic::cpConnectionEstablished(int size)
{
	countCPConnectionEstablished += size;
}

size_t SystemStatistic::cpCloseByHost()
{
	return countCPCloseByHost;
}

void SystemStatistic::cpCloseByHost(int size)
{
	countCPCloseByHost += size;
}

size_t SystemStatistic::cpCloseByPeer()
{
	return countCPCloseByPeer;
}

void SystemStatistic::cpCloseByPeer(int size)
{
	countCPCloseByPeer += size;
}

size_t SystemStatistic::cpRequest()
{
	return countCPRequest;
}

void SystemStatistic::cpRequest(int size)
{
	countCPRequest += size;
}

size_t SystemStatistic::cpUpload()
{
	return countCPUpload;
}

void SystemStatistic::cpUpload(int size)
{
	countCPUpload += size;
}

size_t SystemStatistic::cpResponse()
{
	return countCPResponse;
}

void SystemStatistic::cpResponse(int size)
{
	countCPResponse += size;
}

size_t SystemStatistic::cpStream()
{
	return countCPStream;
}

void SystemStatistic::cpStream( int size )
{
	countCPStream += size;
}

size_t SystemStatistic::cpChunked()
{
	return countCPChunked;
}

void SystemStatistic::cpChunked( int size )
{
	countCPChunked += size;
}

size_t SystemStatistic::cpOdd()
{
	return countCPOdd;
}

void SystemStatistic::cpOdd( int size )
{
	countCPOdd += size;
}

size_t SystemStatistic::cpNormal()
{
	return countCPNormal;
}

void SystemStatistic::cpNormal( int size )
{
	countCPNormal += size;
}

void SystemStatistic::writeLog()
{
	ACE_LOG_MSG->acquire();
	PAS_INFO0("+-----------------------------------------------------------------+");
	PAS_INFO0("|                        System statistic                         |");
	PAS_INFO0("+----------------+------------------------+-----------------------+");
	PAS_INFO1("| Client         | Connection accept      | %21d |", countClientConnectionAccept);
	PAS_INFO1("|                | Close by host          | %21d |", countClientCloseByHost);
	PAS_INFO1("|                | Close by peer          | %21d |", countClientCloseByPeer);
	PAS_INFO1("|                | Request                | %21d |", countClientRequest);
	PAS_INFO1("|                | Upload                 | %21d |", countClientUpload);
	PAS_INFO1("|                | Response               | %21d |", countClientResponse);
	PAS_INFO0("+----------------+------------------------+-----------------------+");
	PAS_INFO1("| CP             | Connection request     | %21d |", countCPConnectionRequest);
	PAS_INFO1("|                | Connection established | %21d |", countCPConnectionEstablished);
	PAS_INFO1("|                | Close by host          | %21d |", countCPCloseByHost);
	PAS_INFO1("|                | Close by peer          | %21d |", countCPCloseByPeer);
	PAS_INFO1("|                | Request                | %21d |", countCPRequest);
	PAS_INFO1("|                | Upload                 | %21d |", countCPUpload);
	PAS_INFO1("|                | Response               | %21d |", countCPResponse);
	PAS_INFO1("|                | Normal body            | %21d |", countCPNormal);
	PAS_INFO1("|                | Odd body               | %21d |", countCPOdd);
	PAS_INFO1("|                | Chunked body           | %21d |", countCPChunked);
	PAS_INFO1("|                | Stream body            | %21d |", countCPStream);
	PAS_INFO0("+----------------+------------------------+-----------------------+");
	ACE_LOG_MSG->release();
}