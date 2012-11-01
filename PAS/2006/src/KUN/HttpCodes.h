#ifndef HTTP_CODE_H
#define HTTP_CODE_H

#define	HTTP_CODE_100_CONTINUE (100)
#define	HTTP_CODE_101_SWITCH (101)

#endif

			/*
			HTTP/1.1 100 Continue
			Server: Microsoft-IIS/5.0
			Date: Tue, 19 Sep 2006 05:22:34 GMT

			HTTP/1.1 302 Object moved
			Server: Microsoft-IIS/5.0
			Date: Tue, 19 Sep 2006 05:22:34 GMT
			Connection: close
			KTF_HTTP_KEY: 72AC9AA30A292368
			Pragma: no-cache
			cache-control: no-store
			Location: UPLoadView.asp?id=126490
			Content-Length: 127
			Content-Type: text/html
			Expires: Tue, 19 Sep 2006 05:21:34 GMT
			Cache-control: private
			*/
			
/*===================================================================
HTTP CODES - 100-101

100 - Continue 
Tells the client that the first part of the request has been received and that it should continue with the rest of the request or ignore if the request has been fulfilled. 

101 - Switching Protocols 
Tells the client that the server will switch protocols to that specified in the Upgrade message header field during the current connection. 


HTTP CODES 200-206

200 - OK 
The request sent by the client was successful. 

201 - Created 
The request was successful and a new resource was created. 

202 - Accepted 
The request has been accepted for processing, but has not yet been processed. 

203 - Non-Authoritative Information 
The returned meta information in the entity-header is not the definitive set as available from the origin server. 

204 - No Content 
The request was successful but does not require the return of an entity-body. 

205 - Reset Content 
The request was successful but the User-Agent should reset the document view that caused the request. 

206 - Partial Content 
The partial GET request has been successful. 


HTTP CODES 300-307

300 - Multiple Choices 
The requested resource has multiple possibilities, each with different locations. 

301 - Moved Permanently 
The resource has permanently moved to a different URI. 

302 - Found 
The requested resource has been found under a different URI but the client should continue to use the original URI. 

303 - See Other 
The requested response is at a different URI and should be accessed using a GET command at the given URI. 

304 - Not Modified 
The resource has not been modified since the last request. 

305 - Use Proxy 
The requested resource can only be accessed through the proxy specified in the location field. 

306 - No Longer Used 
Reserved for future use. 

307 - Temporary Redirect 
The resource has temporarily been moved to a different URI. The client should use the original URI to access the resource in future as the URI may change. 


HTTP CODES 400-417

400 - Bad Request 
The syntax of the request was not understood by the server. 

401 - Not Authorised 
The request needs user authentication 

402 - Payment Required 
Reserved for future use. 

403 - Forbidden 
The server has refused to fulfill the request. 

404 - Not Found 
The document/file requested by the client was not found. 

405 - Method Not Allowed 
The method specified in the Request-Line is not allowed for the specified resource. 

406 - Not Acceptable 
The resource requested is only capable of generating response entities which have content characteristics not specified in the accept headers sent in the request. 

407 - Proxy Authentication Required 
The request first requires authentication with the proxy. 

408 - Request Timeout 
The client failed to sent a request in the time allowed by the server. 

409 - Conflict 
The request was unsuccessful due to a conflict in the state of the resource. 

410 - Gone 
The resource requested is no longer available and no forwarding address is available. 

411 - Length Required 
The server will not accept the request without a valid Content-Length header field. 

412 - Precondition Failed 
A precondition specified in one or more Request-Header fields returned false. 

413 - Request Entity Too Large 
The request was unsuccessful because the request entity is larger than the server will allow. 

414 - Request URI Too Long 
The request was unsuccessful because the URI specified is longer than the server is willing to process. 

415 - Unsupported Media Type 
The request was unsuccessful because the entity of the request is in a format not supported by the requested resource for the method requested. 

416 - Requested Range Not Satisfiable 
The request included a Range request-header field, and not any of the range-specifier values in this field overlap the current extent of the selected resource, and also the request did not include an If-Range request-header field. 

417 - Expectation Failed 
The expectation given in the Expect request-header could not be fulfilled by the server. 


HTTP CODES 500-505

500 - Internal Server Error 
The request was unsuccessful due to an unexpected condition encountered by the server. 

501 - Not Implemented 
The request was unsuccessful because the server can not support the functionality needed to fulfill the request. 

502 - Bad Gateway 
The server received an invalid response from the upstream server while trying to fulfill the request. 

503 - Service Unavailable 
The request was unsuccessful to the server being down or overloaded. 

504 - Gateway Timeout 
The upstream server failed to send a request in the time allowed by the server. 

505 - HTTP Version Not Supported 
The server does not support or is not allowing the HTTP protocol version specified in the request. 

===================================================================*/


