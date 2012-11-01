#include <iostream>
#include <string>
#include <list>
#include <map>

#include <stdio.h>
#include <stdlib.h>

#define	LEN_MNC	(3)
#define	LEN_PHONE_NUM	(15)
#define	LEN_MDN	(15)
#define	LEN_PHONE_MIN	(15)
#define	LEN_IMSI	(15)
#define	LEN_CH_INFO	(3)
#define	LEN_BASE_ID	(7) // 기지국 ID로 추정. 필수 로그는 아님.
#define	LEN_MS_MODEL	(15)
#define	LEN_BROWSER	(15)
#define	LEN_CP_NAME	(31)
#define	LEN_SVC_CODE	(31)
#define	LEN_IPADDR	(15)
#define	LEN_HASH_KEY		(19)
#define	LEN_BILL_INFO		(15)

using namespace std;

typedef unsigned int	intMDN_t;
typedef unsigned int	intIP_t;

class UserInfo
{
public:
	UserInfo(intIP_t ip) {
		intIpAddr = ip;
	}
	
	intIP_t getIP() {
		return intIpAddr;
	}
private:
	int sock;  /**< 단말 연결 socket */
	intIP_t intIpAddr; ///< 단말 IP 주소
	char ipAddr[LEN_IPADDR+1];
	char phoneNum[LEN_PHONE_NUM+1];

};

/*
typedef struct {

} MyType;
*/

list<UserInfo> userList;
map<intMDN_t,  UserInfo*> MDNs;
map<intIP_t,  UserInfo*> IPs;

void doit()
{
	UserInfo a((intMDN_t)1), b((intMDN_t)2);
	
	MDNs[a.getIP()] = &a;
	MDNs[a.getIP()] = &a;
	MDNs[b.getIP()] = &b;

	map<intMDN_t,  UserInfo*>::const_iterator it = MDNs.begin();

	cout << "size= " << MDNs.size() << endl;
	for(it=MDNs.begin(); it != MDNs.end(); it++)
	{
	
		cout <<"Key= "<< it->first << endl;
		cout <<"Val= "<< it->second->getIP() << endl;
	}


	//map<intMDN_t,  UserInfo*>::const_iterator i = 1;
	//MDNs.insert((intIP_t)1, &a); 
	for (int i=0; i<10; i++)
	{
		UserInfo *newuser = new UserInfo(random()% 100);
		userList.push_back(*newuser);
		UserInfo &tmp = userList.back();
		
		//list<UserInfo>::const_iterator userit = userList.find(*newuser);
		printf("org=0x%X,  in List=0x%X\n", newuser, (void *)&tmp);
		printf("org=%d,  in List=%d\n", newuser->getIP(), tmp.getIP());
		
		MDNs[newuser->getIP()] = newuser;
	}
	
	
	cout << "size= " << MDNs.size() << endl;
	for(it=MDNs.begin(); it != MDNs.end(); it++)
	{
	
		cout <<"Key= "<< it->first << endl;
		cout <<"Val= "<< it->second->getIP() << endl;
	}
}

int main(int argc, char *argv[])

{

	doit();

}
