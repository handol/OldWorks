#!/usr/bin/env python

# PAS TEST
# 2006. 11. 10

import socket
import select
import time

Body = '''
<html><body>Good</body></html>
'''

Message = '''HTTP/1.1 200 OK\r
Content-Length: %d\r
\r
%s'''  % (len(Body), Body)




def waitSockInput(sock, waitTime):
	try:
		reads, writes, in_erros = select.select([sock], [], [], waitTime)
		if len(reads) > 0:
			return True
		else:
			return False
	except:
		print "Phone: ERROR in select() sock[%d]" % (sock.fileno())
		return False


			
def do_it(PORT=3333, TIME=1, RECVDELAY=1):
	service = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	service.bind(("", PORT))
	service.listen(1)

	print "listening on port", PORT
	print "new conn arrived. delay %d secs before accept" % TIME
	time.sleep(TIME)

	while 1:
		if not waitSockInput(service, 1.0):
			time.sleep(0.1)
			continue
	
		print "new conn arrived. delay %d secs before accept" % TIME
		time.sleep(TIME)
		channel, info = service.accept()
		print "connection from", info

		print "delay %d secs before recv" % RECVDELAY
		time.sleep(RECVDELAY)
		data = channel.recv(1024)
		print "received data: ", data
		channel.send(Message)
		channel.close()
	
if __name__ == '__main__':
	import sys
	if len(sys.argv) < 4:
		print "Usage: port_num accept_delay recv_delay"
		sys.exit()

	try:
		do_it(int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]))
	except:
		pass




