#include "Common.h"
#include "HttpKTFSSLResult.h"
#include "HttpUrlParser.h"
#include <ace/Message_Block.h>
#include "FastString.hpp"
#include "PasLog.h"

using namespace std;
using namespace HTTP;

KTFSSLResult::KTFSSLResult(void)
{
	clear();
}

KTFSSLResult::~KTFSSLResult(void)
{
}

int KTFSSLResult::parse(const char* srcBuf, size_t srcBufSize)
{
	PAS_TRACE0("KTFSSLResult::parse");
	ASSERT(srcBuf != NULL);
	ASSERT(srcBufSize > 0);

	clear();

	// Çì´õ Ã£±â
	header_t header;
	if(getHeader(&header, srcBuf, srcBufSize) < 0)
	{
		PAS_DEBUG("Not Found KTF SSL Result Header");
		return -1;
	}

	//PAS_DEBUG1("KTFSSLResult::parse >> Header\n%s", header.toStr());

	// Çì´õ ÆÄ½Ì
	bool isStartLine = true;
	int lineEndPos = -1;
	line_t line;
	while((lineEndPos = header.getLine(&line, lineEndPos+1)) >=0)
	{
		line.trim();

		if(line.size() == 0)
			continue;

		if(isStartLine)
		{
			isStartLine = false;
			int result = parseStartLine(line);
			if(result < 0)
			{
				PAS_DEBUG("KTFSSLResult::parse >> StartLine ÆÄ½Ì ½ÇÆÐ");
				return -1;
			}
		}
		else
		{
			int result = parseElement(line);
			if(result < 0)
			{
				PAS_DEBUG("KTFSSLResult::parse >> Header Element ÆÄ½Ì ½ÇÆÐ");
				return -1;
			}
		}
	}

	return 0;
}

int	KTFSSLResult::parseStartLine(const line_t& line)
{
	PAS_TRACE("KTFSSLResult::parseStartLine");
	
	if(line.incaseEqual("RESULT"))
		return 0;

	return -1;
}

void KTFSSLResult::clear()
{
	BaseHeader::clear();
}
