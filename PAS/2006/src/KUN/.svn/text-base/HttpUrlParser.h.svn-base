#ifndef __HTTP_URL_PARSER_H__
#define __HTTP_URL_PARSER_H__

#include "Common.h"
#include "FastString.hpp"
#include "HttpTypes.h"

namespace HTTP
{
	class UrlParser
	{
	public:
		UrlParser(void);
		UrlParser(const char* url);
		~UrlParser(void);

		int parse(const char* url);
		host_t getHost();
		int getPort();
		path_t getPath();
		void clear();
		void test();

	private:
		host_t host;
		int port;
		path_t path;
	};
};

#endif
