#ifndef MY_CONFIG_H
#define MY_CONFIG_H

#define	CFG_VAR_LEN	31
#define	CFG_STRVAL_LEN	255

#define	CFG_INT_VAR	(1)
#define	CFG_STR_VAR	(2)

#include <string.h>
#ifndef WIN32
#include <strings.h>
#endif
#ifdef WIN32
#include "windowsDef.h"
#endif

#include "Comparable.h"
#include "sortedList.h"
//#include "Log_Msg.h"

//Config Variables: Name and Value pair

//#define	ACE_DEBUG(X)	printf X


class CfgVar: public Comparable
{
public:
	char name[CFG_VAR_LEN+1]; //
	char isdefault;
	int	int_val;
	char str_val[CFG_STRVAL_LEN+1];

	CfgVar() : Comparable() {}

	/** integer type의 변수 */
	CfgVar(char *varname, int num_val, char *str, int is_def=0): Comparable(), isdefault(is_def){ 
		strcpy(name, varname);
		int_val = num_val;
		if (str)
			strcpy(str_val, str); 
		else
			str_val[0] = 0;
		
	}

	/** string type의 변수 */
	CfgVar(char *varname, char *str, int is_def=0): Comparable(),  isdefault(is_def) { 
		strcpy(name, varname);
		int_val = 0;		
		if (str)
			strcpy(str_val, str); 
		else
			str_val[0] = 0;
	}
	~CfgVar() {}
		
	int	compare(Comparable *other) { 
		return strcasecmp(this->name, ((CfgVar*)other)->name); 
	}
	
	void print() {
		//ACE_DEBUG((LM_DEBUG, "Config Value : %s = %s\n", this->name, this->str_val));
	}
	
};

class Config {
	public:
	Config(int maxvars);
	~Config();
	int open_home(char *homedir, char *confname, char *def_confname=NULL);
	int open(char *file_name);
	int GetIntVal(char *var_name, int default_val=0);
	char* GetStrVal(char *var_name, char *default_val=NULL);
	
	private:
	sortedList<CfgVar> *VarList;
	int	maxVars;
	
	int insert(char *var_name, char *var_val);
	int search(char *var_name);
	int is_digits(char *str);
};


#endif

