main()
{       
	int retrans = 600;       
	int retry =1;       
	struct hostent *hp;       
	struct in_addr ia;       
	char *name = ¡°localhost¡±;       

	res_init();       
	set_resfield(RES_RETRANS, &retrans);       
	set_resfield(RES_RETRY, &retry);       
	hp = gethostbyname (name);       
	if (hp == NULL ) {           
		printf (¡°gethostbyname failed\n¡±);         
		herror(¡°Error¡±);         
	} else {            
		int i;            
		for (i=o; hp->h_addr_list[i]; i++) {              
			memcpy((caddr_t)&ia, hep->h_addr_list[i],sizeof(ia));              
			printf(¡°%s¡±, inet_ntoa(ia));            
		}
	}

	get_resfield (RES_RETRANS, &retrans, sizeof retrans);   
	get_resfield (RES_RETRY, &retry, sizeof retry);   
	printf (¡°retry = %d \n retrans = %d\n¡±, retry,retrans);
}
