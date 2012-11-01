import CGIHTTPServer, BaseHTTPServer
#import daemon 
class CGIHandler(CGIHTTPServer.CGIHTTPRequestHandler): 
    cgi_directories = ['/mycgi'] 
#daemon.daemonize() 

if __name__ == "__main__":
	import sys
	if len(sys.argv) > 1:
		port = int(sys.argv[1])
	else:
		port = 50000

	BaseHTTPServer.HTTPServer(('', port), CGIHandler).serve_forever() 

