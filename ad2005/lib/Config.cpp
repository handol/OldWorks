
#include "ace/Log_Msg.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#ifndef WIN32
#include <strings.h>
#endif

#include "Config.h"

#include "Spliter.h"


 Config::Config(int _maxvars )
{
	maxVars = _maxvars;
	VarList = new sortedList<CfgVar> (maxVars);

	if (VarList==NULL) {
		ACE_DEBUG((LM_INFO, "Config:: VarList allocation failed\n"));
	}
	
}

Config::~Config()
{
	if (VarList) delete VarList;
}


/**
구현할 시스템의 home dir, 주어진 config 화일 이름, default config 화일 이름을 가지고 최종 path값을 구한다.


 home dir이 주어진 경우 home dir 아래 cfg 에서 config 화일을 찾는다.
*/
int Config::open_home(char *homedir, char *confname, char *def_confname) 
{
	char	confpath[256]={0};

	if (confname[0]==0 && def_confname) {
		strcpy(confname, def_confname);
	}
	
	if (homedir) {
		sprintf(confpath, "%s/%s", homedir, confname);	
		
	}
	else {
		sprintf(confpath, "%s", confname);
	}

	return this->open(confpath);
}

/**
config 화일을 읽어 name - value 의 pair 를 목록에 저장 (insert) 한다.

*/
int Config::open(char *fname) 
{
  char buf[256];
  FILE * fp;
  char var_name[CFG_VAR_LEN+1];
  char var_value[CFG_STRVAL_LEN+1];

  int lineno = 0;

  if ((fp = fopen(fname, "r")) == NULL) {
    ACE_DEBUG((LM_INFO, "Cannot read the CONFIG file : %s\n", fname));
    return (-1);
  }
  ACE_DEBUG((LM_INFO, "reading CONFIG file : %s\n", fname));
  while (fgets(buf, 256, fp) != NULL) {
		int	sep=0;
		Spliter	split(buf, strlen(buf), ' ');
		
		lineno++;

		/* var name part */
		sep = split.getNext(var_name, CFG_VAR_LEN, "=\n");
		Spliter::trim(var_name, var_name);
		if (var_name[0]==0 || var_name[0]=='#') continue;

		if (sep != '=') {
			ACE_DEBUG((LM_INFO, "Config error: line[%d], Value missing. Config Variable : %s\n",
		   	 lineno, var_name));
			continue;
		}

		split.getNext(var_value, CFG_STRVAL_LEN, "=\n");
		Spliter::trim(var_value, var_value);
		
		if (var_value[0]==0) {
			ACE_DEBUG((LM_INFO, "Config error: line[%d], Config Variable : %s\n",
		   	 lineno, var_name));
			continue;
		}
		
		ACE_DEBUG((LM_INFO, "Config Value : %s = %s\n", var_name, var_value));
		if (is_digits(var_value)) {
			CfgVar  newone(var_name, strtol(var_value, 0, 10), var_value);
			VarList->insert( &newone);
		}
		else {
			CfgVar  newone(var_name, var_value);
			VarList->insert( &newone);
		}

  }

	ACE_DEBUG((LM_INFO, "CONFIG: %d values\n", VarList->count()));
  /* close log file */ 
  fclose(fp);
  return (0);
}

int Config::is_digits(char *str)
{
	while(*str)
		if (! isdigit(*str++)) return 0;
	return 1;
}

/**
주어진 이름을 가진 config value를 찾아 return한다. 
없는 경우 주어진 default value를 목록에 insert 하고, 그값을 return 한다.
*/
int Config::GetIntVal(char *var_name, int default_val)
{
	CfgVar *found=NULL;
	CfgVar srch(var_name, default_val, " ", TRUE);
	found = VarList->search(&srch);
	if (found) return found->int_val;

	if (default_val) {
		VarList->insert(&srch);
		return default_val;
	}
	else
		return 0;	
}

/**
주어진 이름을 가진 config value를 찾아 return한다. 
없는 경우 주어진 default value를 목록에 insert 하고, 그값을 return 한다.
주어진 이름의 변수가 없는 경우 빈 문자열(not NULL point)을 return 한다.
*/
char* Config::GetStrVal(char *var_name, char *default_val)
{
	CfgVar *found=NULL;
	CfgVar srch(var_name, default_val, TRUE);
	found = VarList->search(&srch);
	if (found) return found->str_val;

	if (default_val) {
		found = VarList->insert(&srch);
		if (found) return found->str_val;
		else	return default_val;
	}
	else {
		return "";
	}
}
