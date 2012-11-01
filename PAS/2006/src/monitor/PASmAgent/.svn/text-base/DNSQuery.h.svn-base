//////////////////////////////////////////////////////////////////////
//
// DNSQuery.h: interface for the CDNSQuery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DNSQUERY_H__0EC19355_16E4_41B5_B917_2E810A322503__INCLUDED_)
#define DNSQUERY_H__0EC19355_16E4_41B5_B917_2E810A322503__INCLUDED_

class CDNSQuery  
{
private:
	int bGetHostByName(char* sName, char* sIP);
	char m_svrip[128];
public:
	char* GetIP();
	in_addr_t lGetIP(char* sAddr);	
	CDNSQuery();
	virtual ~CDNSQuery();
};

#endif // !defined(DNSQUERY_H__0EC19355_16E4_41B5_B917_2E810A322503__INCLUDED_)
