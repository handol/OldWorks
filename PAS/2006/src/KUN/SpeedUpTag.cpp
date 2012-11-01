#include <stdio.h>
#include <string.h>

#include "SpeedUpTag.h"

SpeedUpTag::SpeedUpTag(void)
{
}

SpeedUpTag::~SpeedUpTag(void)
{
}

long SpeedUpTag::isCompString2( const char *pSource, const char *pszComp, long *pPos, int icase )
{
	*pPos = 0;
	long count = 0;

	while (*pSource != NULL)
	{
		if ( *pSource == *pszComp )
		{
			if ( icase == 1 )
			{
				if ( !strncasecmp(pSource, pszComp, strlen(pszComp)) )
				{
					return 1;
				}
			}
			else
			{
				if ( !strncmp(pSource, pszComp, strlen(pszComp)) )
				{
					return 1;
				}
			}
		}

		*pSource++;
		*pPos = ++count;
	}

	return 0;
}

int SpeedUpTag::insert( char *pszHTML, int iSize )
{
	if ( pszHTML == NULL && iSize < 1 )
		return 0;

	// HTML Size가 작다면???
	char* pbuff = new char[iSize*2 + 1024];
	memset(pbuff, 0x00, iSize*2 + 1024);

	long Pos = 0;
	long SPos = 0;
	long nOffset = 0;
	long nSize = 0;
	while(isCompString2(pszHTML + nOffset, "<a ", &SPos, 1))
	{
		long EPos = 0;
		long LPos = 0;
		if ( isCompString2(pszHTML + nOffset + SPos, ">", &EPos, 1) )
		{
			//Tag Invalidation Check
			int iRet = isCompString2(pszHTML + nOffset + SPos+strlen("<a"), "<", &LPos, 1);

			if ( iRet == 1 && LPos > 0 && LPos < EPos )
			{
				memcpy(pbuff + nSize, pszHTML + nOffset, SPos + EPos);
				nOffset += SPos + EPos;
				nSize += SPos + EPos;
				continue;
			}

			if ( isCompString2(pszHTML + nOffset, " speedup", &Pos, 1) )
			{
				if ( Pos > SPos && Pos < (EPos + SPos) )
				{
					memcpy(pbuff + nSize, pszHTML + nOffset, SPos + EPos);
					nOffset += SPos + EPos;
					nSize += SPos + EPos;
					continue;
				}
			}

			memcpy(pbuff + nSize, pszHTML + nOffset, SPos + EPos);
			memcpy(pbuff + nSize + SPos + EPos, " speedup>", 9);

			nOffset += SPos + EPos + 1;
			nSize += SPos + EPos + 9;
		}
		else
		{
			break;
		}
	}

	memcpy(pbuff + nSize, pszHTML + nOffset, iSize - nOffset);
	strcpy(pszHTML, pbuff);

	if ( pbuff )
	{
		delete[] pbuff;
		pbuff = NULL;
	}

	return nSize + iSize - nOffset;
}

int SpeedUpTag::remove( char *pszHTML, int iSize )
{
	if ( pszHTML == NULL && iSize < 1 )
		return 0;

	// HTML Size가 작다면???
	char* pbuff = new char[iSize+1];
	memset(pbuff, 0x00, iSize+1);

	long SPos = 0;
	long nOffset = 0;
	long nSize = 0;

	while(isCompString2(pszHTML + nOffset, "<a ", &SPos, 1))
	{
		long EPos = 0;
		long Pos = 0;
		long LPos = 0;

		if ( isCompString2(pszHTML + nOffset + SPos, ">", &EPos, 1) )
		{
			//Tag Invalidation Check
			int iRet = isCompString2(pszHTML + nOffset + SPos+strlen("<a>"), "<", &LPos, 1);

			if ( iRet == 1 && LPos > 0 &&  LPos < EPos )
			{
				memcpy(pbuff + nSize, pszHTML + nOffset, SPos + EPos);
				nOffset += SPos + EPos;
				nSize += SPos + EPos;
				continue;
			}

			if ( isCompString2(pszHTML + nOffset + SPos, " speedup", &Pos, 1) )
			{
				if ( Pos < EPos )
				{
					memcpy(pbuff + nSize, pszHTML + nOffset, SPos + Pos);
					memcpy(pbuff + nSize + SPos + Pos, pszHTML + nOffset + SPos + Pos + 8, EPos - Pos - 7);
				}
				else
				{
					memcpy(pbuff + nSize, pszHTML + nOffset, SPos + EPos);
					nOffset += SPos + EPos;
					nSize += SPos + EPos;

					continue;
				}
			}
			else
			{
				memcpy(pbuff + nSize, pszHTML + nOffset, SPos + EPos);
				nOffset += SPos + EPos;
				nSize += SPos + EPos;

				continue;
			}

			nOffset += SPos + Pos + 8 + EPos - Pos -7;
			nSize += SPos + Pos + EPos - Pos - 7;
		}
		else
		{
			break;
		}
	}

	memcpy(pbuff + nSize, pszHTML + nOffset, iSize - nOffset);
	strcpy(pszHTML, pbuff);

	if ( pbuff )
	{
		delete[] pbuff;
		pbuff = NULL;
	}

	return nSize + iSize - nOffset;
}


