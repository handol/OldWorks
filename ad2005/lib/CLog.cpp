	#include "CLog.h"

CLog::CLog(char *dir_name, char *module_name)
{
	char *ptr=0;
  logfd = 0;
  errfd = 0;
  t_val = 0;
  t = 0;
  old_day = 0;

  if (dir_name == 0 || module_name == 0) {
    fName[0] = 0;
    return;
  }
  	/**
	if ((ptr=strrchr(module_name, '/'))!=0)
  sprintf(fName, "%s/%s", dir_name, ptr+1);
	else
	*/
  sprintf(fName, "%s/%s", dir_name, module_name);

}

CLog::~CLog()
{
  if (logfd)  fclose(logfd);
  if (errfd)  fclose(errfd);  
}

// ���ڰ� �ٲ���ų� ȭ�� open�� �ȵ� ��� ���� ȭ���� ����. 
FILE *CLog::openLog(FILE * fp, char *surfix)
{
  char logf_path[256];

  time(&t_val);
  t = localtime(&t_val);
  if (fp == 0 || old_day != t->tm_mday) {
    old_day = t->tm_mday;

    if (fp) fclose(fp);

    if (surfix)
      //sprintf(logf_path, "%s.%s.%02d%02d", surfix, fName, t->tm_mon + 1, t->tm_mday);
      sprintf(logf_path, "%s.%s.%02d%02d", fName, surfix, t->tm_mon + 1, t->tm_mday);
    else
      sprintf(logf_path, "%s.%02d%02d", fName, t->tm_mon + 1, t->tm_mday);
    fp = fopen(logf_path, "a");

  }
  return fp;
}

// �ð�ǥ�� ���� ��� 
void CLog::LogPRN2(char *fmt, ...)
{

  logfd = openLog(logfd, 0);
  if (logfd == 0)   return;

  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);

  fprintf(logfd, "%s", buf);
  fflush(logfd);
}

void CLog::LogPRN(char *fmt, ...)
{

  logfd = openLog(logfd, 0);
  if (logfd == 0)
    return;

  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);

  fprintf(logfd, "%02d:%02d:%02d %s", t->tm_hour, t->tm_min, t->tm_sec,
	  buf);
  fflush(logfd);
}


// Error log ȭ���� ���� �ִ� ��� 

void CLog::ePRN(char *fmt, ...)
{
  errfd = openLog(errfd, "error");
  if (errfd == 0) {
    goto GEN_LOG;
  }

  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);

  fprintf(errfd, "%02d:%02d:%02d %s", t->tm_hour, t->tm_min, t->tm_sec,
	  buf);
  fflush(errfd);
  return;

GEN_LOG:
  logfd = openLog(logfd, 0);
  if (logfd == 0)
    return;
  fprintf(logfd, "%02d:%02d:%02d [ERROR] %s",
	  t->tm_hour, t->tm_min, t->tm_sec, buf);
  fflush(logfd);

  return;
}

void CLog::ePRN2(char *fmt, ...)
{  
  logfd = openLog(logfd, 0);
  if (logfd == 0)
    return;
  fprintf(logfd, "%02d:%02d:%02d [ERROR] %s",
	  t->tm_hour, t->tm_min, t->tm_sec, buf);
  fflush(logfd);

  return;
}


