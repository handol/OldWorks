#ifndef THREAD_DEF_H
#define THREAD_DEF_H

/*

AIX system call maual 


http://publib.boulder.ibm.com/infocenter/pseries/index.jsp?topic=/com.ibm.aix.resources/downloads.htm
*/

#ifdef USE_PTHREAD


	/* AIX system에 /usr/include/sys/types.h 에 정의가 되어 있으나  compiler가 제대로 포함을 못함. */
	/* 에러 메시지.
		/usr/include/sys/proc.h:203: error: 'crid_t' is used as a type, but is not   defined as a type.
		/usr/include/sys/proc.h:212: error: 'class_id_t' is used as a type, but is not   defined as a type.
   	*/
	
	#include <sys/types.h>

	#ifndef crid_t
	typedef int         crid_t;
	#endif

	#ifndef class_id_t
	typedef unsigned int class_id_t;
	#endif
	


#include <pthread.h>
#define THR_CREATE(A, B, C, D)  pthread_create
#define THR_EXIT  pthread_exit
#define THR_SELF  pthread_self
#define THR_MUTEX_T pthread_mutex_t
#define THR_T pthread_t

#else /* if not USE_PTHREAD */

/*
// AIX
#include <sys/thread.h>
#define THR_CREATE thread_create
#define THR_EXIT  thread_terminate 
#define THR_SELF  thread_self
#define THR_MUTEX_T mutex_t
*/

#include <thread.h>
#define THR_CREATE thread_create
#define THR_EXIT  thread_terminate 
#define THR_SELF  thr_self
#define THR_MUTEX_T mutex_t


#endif /* USE_PTHREAD */

#endif /* THREAD_DEF_H */
