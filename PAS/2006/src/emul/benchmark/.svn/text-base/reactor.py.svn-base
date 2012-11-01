# -*- coding: EUC-KR -*-
#

import select
import log
import threading
import thread

mutex = thread.allocate_lock()

#---------------------------------------------------------
# reactor for Receiving
class reactorR(threading.Thread):
	def __init__(self, debug=0):
		threading.Thread.__init__(self)

		self.handlers = {} ## dictionary of handler object
		self.handles = [] ## list of socket fd
		self.isStop = False

	def register(self, handler):
		global mutex

		mutex.acquire()
		try:
			self.handlers[handler.sock] = handler
			self.handles.append(handler.sock)
			#print 'reactor: Regi %d' % (handler.sock.fileno())
		except:
			pass
		mutex.release()
		
	def unregister(self, handler):
		global mutex
		mutex.acquire()
		try:
			del self.handlers[handler.sock]
			self.handles.remove(handler.sock)
			#print 'reactor: Unregi %d' % (handler.sock.fileno())
		except:
			pass
		mutex.release()
	
	def info(self):
		print "Number of handlers = %d" % self.getHandlers()
		
	def getHandlers(self):
		global mutex
		mutex.acquire()
		num = len(self.handlers)
		mutex.release()
		return num

	def proc(self):

		if(self.getHandlers() > 0):
			waitTime = 5
			try:
				reads, writes, in_erros = select.select(self.handles, [], [], waitTime)
			except:
				reads = []
				
			for sock in reads:
				#print "reactor: event on sock %d" % (sock.fileno())
				res = self.handlers[sock].handle_input()
				if res < 0:
					self.unregister(self.handlers[sock])
			
			if len(reads) == 0:
				print "reactor: select timeout"
				self.stop()

	def stop(self):
		self.isStop = True

	def run(self):
		print "reactor run() starts"
		while 1:
			#self.info()
			self.proc()
			if self.isStop:
				break
		print "reactor stop()"


