#ifndef CLOG_H
#define	CLOG_H
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/timeb.h>
#include <time.h>

class CLog {
public:
  void LogPRN2(char *fmt, ...);
  void LogPRN(char *fmt, ...);
  void ePRN(char *fmt, ...);
  void ePRN2(char *fmt, ...);

  ~CLog();
   CLog(char *dir_name, char *module_name);

  char fName[256];
private:
   FILE * openLog(FILE * fp, char *surfix);
  va_list args;
  char buf[1024*10];

  FILE *logfd;
  FILE *errfd;
  time_t t_val;
  struct tm *t;
  int old_day;
};
#endif
