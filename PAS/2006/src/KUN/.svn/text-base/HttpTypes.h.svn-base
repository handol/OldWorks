#ifndef __HTTP_TYPES_H__
#define __HTTP_TYPES_H__

#include <list>
#include "FastString.hpp"

namespace HTTP
{
	struct HeaderElement
	{
		typedef FastString<256> key_t;
		typedef FastString<2048> value_t;

		HeaderElement()
		{
		}

		HeaderElement(const key_t& ikey, const value_t& ivalue)
			: key(ikey), value(ivalue)
		{
		}

		key_t key;
		value_t value;
	};

	enum RequestMethod
	{
		RM_NONE,
		RM_GET,
		RM_PUT,
		RM_DELETE,
		RM_POST,		
		RM_HEAD,
		RM_TRACE,
		RM_OPTIONS,
		RM_CONNECT,
		RM_RESULT
	};

	enum ResponseStatus
	{
		RS_NONE = 0,
		RS_INFORMATION = 100,
		RS_SUCCESS = 200,
		RS_REDIRECTION = 300,
		RS_CLIENT_ERROR = 400,
		RS_SERVER_ERROR = 500,
		RS_UNKNOWN = 9999
	};

	typedef HeaderElement::key_t key_t;
	typedef HeaderElement::value_t value_t;

	typedef FastString<64> chunkSize_t;
	typedef FastString<10240> header_t;
	typedef FastString<32> method_t;
	typedef FastString<32> version_t;
	typedef FastString<32> status_t;
	typedef FastString<1024> path_t;

	typedef std::vector<HeaderElement> HeaderElements;
	typedef std::vector<HeaderElement::key_t> Keys;
};

#endif
