#ifndef __PAS_LOG_H__
#define __PAS_LOG_H__

#include <ace/Log_Msg.h>

#define PAS_TRACE(A)				PAS_TRACE0(A)
#define PAS_DEBUG(A)				PAS_DEBUG0(A)
#define PAS_INFO(A)					PAS_INFO0(A)	
#define PAS_NOTICE(A)				PAS_NOTICE0(A)	
#define PAS_ALERT(A)				PAS_ALERT0(A)	
#define PAS_WARNING(A) 				PAS_WARNING0(A)	
#define PAS_ERROR(A)				PAS_ERROR0(A)	

#define PAS_TRACE0(A)				ACE_DEBUG((LM_TRACE,		ACE_TEXT("[%T:%2t:TRAC:%19s:%4d] " A "\n"), __FILE__, __LINE__))
#define PAS_DEBUG0(A)				ACE_DEBUG((LM_DEBUG,		ACE_TEXT("[%T:%2t:DBUG:%19s:%4d] " A "\n"), __FILE__, __LINE__))
#define PAS_INFO0(A)				ACE_DEBUG((LM_INFO,			ACE_TEXT("[%T:%2t:INFO:%19s:%4d] " A "\n"), __FILE__, __LINE__))
#define PAS_NOTICE0(A)				ACE_DEBUG((LM_NOTICE,		ACE_TEXT("[%T:%2t:NOTI:%19s:%4d] " A "\n"), __FILE__, __LINE__))
#define PAS_WARNING0(A)				ACE_DEBUG((LM_WARNING,		ACE_TEXT("[%T:%2t:WARN:%19s:%4d] " A "\n"), __FILE__, __LINE__))
#define PAS_ERROR0(A)				ACE_DEBUG((LM_ERROR,		ACE_TEXT("[%T:%2t:ERRO:%19s:%4d] " A "\n"), __FILE__, __LINE__))

#define PAS_TRACE1(A,B)				ACE_DEBUG((LM_TRACE,		ACE_TEXT("[%T:%2t:TRAC:%19s:%4d] " A "\n"), __FILE__, __LINE__, B))
#define PAS_DEBUG1(A,B)				ACE_DEBUG((LM_DEBUG,		ACE_TEXT("[%T:%2t:DBUG:%19s:%4d] " A "\n"), __FILE__, __LINE__, B))
#define PAS_INFO1(A,B)				ACE_DEBUG((LM_INFO,			ACE_TEXT("[%T:%2t:INFO:%19s:%4d] " A "\n"), __FILE__, __LINE__, B))
#define PAS_NOTICE1(A,B)			ACE_DEBUG((LM_NOTICE,		ACE_TEXT("[%T:%2t:NOTI:%19s:%4d] " A "\n"), __FILE__, __LINE__, B))
#define PAS_WARNING1(A,B)			ACE_DEBUG((LM_WARNING,		ACE_TEXT("[%T:%2t:WARN:%19s:%4d] " A "\n"), __FILE__, __LINE__, B))
#define PAS_ERROR1(A,B)				ACE_DEBUG((LM_ERROR,		ACE_TEXT("[%T:%2t:ERRO:%19s:%4d] " A "\n"), __FILE__, __LINE__, B))

#define PAS_TRACE2(A,B,C)			ACE_DEBUG((LM_TRACE,		ACE_TEXT("[%T:%2t:TRAC:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C))
#define PAS_DEBUG2(A,B,C)			ACE_DEBUG((LM_DEBUG,		ACE_TEXT("[%T:%2t:DBUG:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C))
#define PAS_INFO2(A,B,C)			ACE_DEBUG((LM_INFO,			ACE_TEXT("[%T:%2t:INFO:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C))
#define PAS_NOTICE2(A,B,C)			ACE_DEBUG((LM_NOTICE,		ACE_TEXT("[%T:%2t:NOTI:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C))
#define PAS_WARNING2(A,B,C)			ACE_DEBUG((LM_WARNING,		ACE_TEXT("[%T:%2t:WARN:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C))
#define PAS_ERROR2(A,B,C)			ACE_DEBUG((LM_ERROR,		ACE_TEXT("[%T:%2t:ERRO:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C))

#define PAS_TRACE3(A,B,C,D)			ACE_DEBUG((LM_TRACE,		ACE_TEXT("[%T:%2t:TRAC:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D))
#define PAS_DEBUG3(A,B,C,D)			ACE_DEBUG((LM_DEBUG,		ACE_TEXT("[%T:%2t:DBUG:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D))
#define PAS_INFO3(A,B,C,D)			ACE_DEBUG((LM_INFO,			ACE_TEXT("[%T:%2t:INFO:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D))
#define PAS_NOTICE3(A,B,C,D)		ACE_DEBUG((LM_NOTICE,		ACE_TEXT("[%T:%2t:NOTI:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D))
#define PAS_WARNING3(A,B,C,D)		ACE_DEBUG((LM_WARNING,		ACE_TEXT("[%T:%2t:WARN:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D))
#define PAS_ERROR3(A,B,C,D)			ACE_DEBUG((LM_ERROR,		ACE_TEXT("[%T:%2t:ERRO:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D))

#define PAS_TRACE4(A,B,C,D,E)		ACE_DEBUG((LM_TRACE,		ACE_TEXT("[%T:%2t:TRAC:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E))
#define PAS_DEBUG4(A,B,C,D,E)		ACE_DEBUG((LM_DEBUG,		ACE_TEXT("[%T:%2t:DBUG:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E))
#define PAS_INFO4(A,B,C,D,E)		ACE_DEBUG((LM_INFO,			ACE_TEXT("[%T:%2t:INFO:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E))
#define PAS_NOTICE4(A,B,C,D,E)		ACE_DEBUG((LM_NOTICE,		ACE_TEXT("[%T:%2t:NOTI:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E))
#define PAS_WARNING4(A,B,C,D,E)		ACE_DEBUG((LM_WARNING,		ACE_TEXT("[%T:%2t:WARN:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E))
#define PAS_ERROR4(A,B,C,D,E)		ACE_DEBUG((LM_ERROR,		ACE_TEXT("[%T:%2t:ERRO:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E))

#define PAS_TRACE5(A,B,C,D,E,F)		ACE_DEBUG((LM_TRACE,		ACE_TEXT("[%T:%2t:TRAC:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E, F))
#define PAS_DEBUG5(A,B,C,D,E,F)		ACE_DEBUG((LM_DEBUG,		ACE_TEXT("[%T:%2t:DBUG:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E, F))
#define PAS_INFO5(A,B,C,D,E,F)		ACE_DEBUG((LM_INFO,			ACE_TEXT("[%T:%2t:INFO:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E, F))
#define PAS_NOTICE5(A,B,C,D,E,F)	ACE_DEBUG((LM_NOTICE,		ACE_TEXT("[%T:%2t:NOTI:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E, F))
#define PAS_WARNING5(A,B,C,D,E,F)	ACE_DEBUG((LM_WARNING,		ACE_TEXT("[%T:%2t:WARN:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E, F))
#define PAS_ERROR5(A,B,C,D,E,F)		ACE_DEBUG((LM_ERROR,		ACE_TEXT("[%T:%2t:ERRO:%19s:%4d] " A "\n"), __FILE__, __LINE__, B, C, D, E, F))

#define PAS_TRACE_DUMP(STR, BUF, SIZE)		ACE_HEX_DUMP((LM_TRACE, BUF, SIZE, ACE_TEXT(""STR"")))
#define PAS_DEBUG_DUMP(STR, BUF, SIZE)		ACE_HEX_DUMP((LM_DEBUG, BUF, SIZE, ACE_TEXT(""STR"")))
#define PAS_INFO_DUMP(STR, BUF, SIZE)		ACE_HEX_DUMP((LM_INFO, BUF, SIZE, ACE_TEXT(""STR"")))
#define PAS_NOTICE_DUMP(STR, BUF, SIZE)		ACE_HEX_DUMP((LM_NOTICE, BUF, SIZE, ACE_TEXT(""STR"")))
#define PAS_WARNING_DUMP(STR, BUF, SIZE)	ACE_HEX_DUMP((LM_WARNING, BUF, SIZE, ACE_TEXT(""STR"")))
#define PAS_ERROR_DUMP(STR, BUF, SIZE)		ACE_HEX_DUMP((LM_ERROR, BUF, SIZE, ACE_TEXT(""STR"")))

#endif
