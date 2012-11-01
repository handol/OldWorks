#!/usr/local/bin/python
# -*- coding: EUC-KR -*-
#

#----------------------------------------------------------------------------
TargetClients = 1
MinClients = -1
MaxClients = -1
StepClients = -1
IntervalTime = -1
LastTime = -1

RequestPerConnection = 100
PipelineNum = 1
MaxRequests = 0
HostIP = "192.168.111.2"
Port = 80
IsRun = True
RequestSleepTime = 0.0

#----------------------------------------------------------------------------
import reactor
import eh
import signal
import log

#----------------------------------------------------------------------------
def getUrlList():
	lines = []
	fd = file("urlList.txt", "r")

	while True:
		line = fd.readline()
		if len(line) == 0:
			break;

		if line[0] != "#":
			lines.append(line.rstrip("\r\n"))

	fd.close()

	return lines

#----------------------------------------------------------------------------
def ToSizeBasedResponseTime(runTimeInfo):
	results = {}

	# v[0] : URL
	# v[1] : elapsed time
	# v[2] : request file size
	for v in runTimeInfo:
		if results.has_key(v[2]):
			results[v[2]][0] += v[1]
			results[v[2]][1] += 1
			results[v[2]][2] = min((results[v[2]][2], v[1]))
			results[v[2]][3] = max((results[v[2]][3], v[1]))
		else:
			results[v[2]] = [v[1],1,v[1],v[1]]

	# i             : request file size
	# results[i][0] : sum of elapsed time
	# results[i][1] : number of samples
	# results[i][2] : min elapsed time
	# results[i][3] : max elapsed time
	return results

#----------------------------------------------------------------------------
def toHumanReadableNumber(num):
	sizeK = 0
	sizeM = 0
	newNum = num
	if num >= 1024:
		sizeK = num / 1024
		newNum = "%dK" % sizeK
		if sizeK >= 1024:
			sizeM = sizeK / 1024
			newNum = "%dM" % sizeM

	return newNum

#----------------------------------------------------------------------------
def saveSizeBasedTime(filename, runTimeInfo, strSummary):
	fd = file(filename, "w")
 	line = "%10s, %8s, %17s, %17s, %17s, %8s\n" % \
 		("size", "size2", "ave. elapsed time", "min elapsed time", "max elapsed time", "requests")
 	fd.writelines(line)

	results = ToSizeBasedResponseTime(runTimeInfo)

	keys = results.keys()
	keys.sort()

	for v in keys:
		size2 = toHumanReadableNumber(v)
		line = "%10d, %8s, %17f, %17f, %17f, %8d\n"  % \
			(v, size2, results[v][0]/results[v][1], results[v][2], results[v][3], results[v][1])
		fd.writelines(line)

	fd.writelines("\n\n%s\n" % (strSummary))
	fd.close()

#----------------------------------------------------------------------------
def saveRunTimeInfo(filename, runTimeInfo):
	fd = file(filename, "w")

 	line = "%12s, %8s, %25s, %25s, %s\n" % ("Elapsed Time", "Size", "StartTime", "EndTime", "URL")
 	fd.writelines(line)

	# v[0] : URL
	# v[1] : elapsed time
	# v[2] : request file size
	# v[3] : start time
	# v[4] : end time
	for v in runTimeInfo:

		strStartTime = time.strftime("%c", time.localtime(v[3]))
		# 시간 소수부분 추가
		strFloat = "%f" % v[3]
		pos = strFloat.find(".")
		strStartTime += strFloat[pos:pos+4]

		strEndTime = time.strftime("%c", time.localtime(v[4]))
		# 시간 소수부분 추가
		strFloat = "%f" % v[4]
		pos = strFloat.find(".")
		strEndTime += strFloat[pos:pos+4]

		line = "%12.3f, %8d, %25s, %25s, %s\n" % (v[1], v[2], strStartTime, strEndTime, v[0])
		fd.writelines(line)

	fd.close()

#----------------------------------------------------------------------------
def getSummaryStats(runTimeInfo, programRunTime, numberOfConnection, targetClients, countConFail, countTimeout):
	global RequestPerConnecion

	totalTime = 0.0
	totalRecvSize = 0

	# v[0] : URL
	# v[1] : elapsed time
	# v[2] : request file size
	for v in runTimeInfo:
		totalTime += v[1]
		totalRecvSize += v[2]

	if len(runTimeInfo) > 0:
		averageRequestTime = totalTime/len(runTimeInfo)
	else:
		averageRequestTime = 0

	totalConn = numberOfConnection + countConFail

	stats = {}
	stats["RunTime"] = programRunTime
	stats["Client"] = targetClients
	stats["ReqPerConn"] = RequestPerConnection
	stats["ConnSuccess"] = numberOfConnection
	stats["ConnFailure"] = countConFail
	stats["Timeout"] = countTimeout
	stats["Request"] = len(runTimeInfo)
	stats["TotalReqTime"] = totalTime
	stats["TotalRecvSize"] = totalRecvSize

	return stats

#----------------------------------------------------------------------------
def saveSumStats(sumStats):
	global RequestSleepTime

	fd = file("sumSummary.csv", "w")
 	line = "RunTime, Client, ReqPerConn, ConnSuccess, ConnFailure, Timeout, Request, AvgReqTime, RecvSizePerSec, AvgReqPerSec, ReqSleepTime\n"
 	fd.writelines(line)

	for s in sumStats:
		try:
			AvgReqTime = s["TotalReqTime"] / s["Request"]
			RecvSizePerSec = s["TotalRecvSize"] / s["RunTime"]
			AvgReqPerSec = s["Request"] / s["RunTime"]

			line = "%7d, %6d, %10d, %11d, %11d, %7d, %7d, %10f, %14d, %12d, %12f\n"  % \
				(s["RunTime"], s["Client"], s["ReqPerConn"], s["ConnSuccess"], s["ConnFailure"], s["Timeout"], s["Request"], AvgReqTime, RecvSizePerSec, AvgReqPerSec, RequestSleepTime)

			fd.writelines(line)
		except:
			pass

	fd.close()


#----------------------------------------------------------------------------
def getSummary(runTimeInfo, programRunTime, numberOfConnection, targetClients, countConFail, countTimeout):
	global RequestPerConnecion
	global RequestSleepTime

	totalTime = 0.0
	totalRecvSize = 0

	# v[0] : URL
	# v[1] : elapsed time
	# v[2] : request file size
	for v in runTimeInfo:
		totalTime += v[1]
		totalRecvSize += v[2]

	if len(runTimeInfo) > 0:
		averageRequestTime = totalTime/len(runTimeInfo)
	else:
		averageRequestTime = 0

	totalConn = numberOfConnection + countConFail

	str = ''
	str += "Program Running Time       : %f\n" % (programRunTime)
	str += "Number of Clients          : %d\n" % (targetClients)
	str += "Request Per Connection     : %d\n" % (RequestPerConnection)
	str += "Request Sleep Time         : %f\n" % (RequestSleepTime)
	try:
		str += "Connection Success         : %d (%.2f%%)\n" % (numberOfConnection, float(numberOfConnection)/float(totalConn)*100.0)
	except:
		pass

	try:
		str += "Connection Failure         : %d (%.2f%%)\n" % (countConFail, float(countConFail)/float(totalConn)*100.0)
	except:
		pass

	try:
		str += "Receive Timeout            : %d (%.2f%%)\n" % (countTimeout, float(countTimeout)/float(len(runTimeInfo))*100.0)
	except:
		pass

	str += "Total Success Count        : %d\n" % (len(runTimeInfo))
	str += "Total Request Time         : %f\n" % (totalTime)
	str += "Total Receive Size(MB)     : %f\n" % (totalRecvSize / (1024*1024))
	str += "MegaByte Per Second        : %f\n" % (totalRecvSize / (1024*1024) / programRunTime)
	str += "Average Time Per Request   : %f\n" % (averageRequestTime)
	str += "Average Request Per Second : %f\n" % (len(runTimeInfo)/programRunTime)

	return str

#----------------------------------------------------------------------------
def programStop(sigNum, frame):
	global IsRun

	print "Program Stopping, Please Wait.........."
	IsRun = False

#----------------------------------------------------------------------------
def parseParam():
	global HostIP
	global Port
	global TargetClients
	global RequestPerConnection
	global MaxRequests
	global MinClients
	global MaxClients
	global StepClients
	global IntervalTime
	global LastTime
	global RequestSleepTime
	global PipelineNum

	if len(sys.argv) < 3:
		print "usage: %s HostIP PortNumber [ClientNumber=%d or Min/Max/Step/IntervalTime/LastTime] [RequestPerConnection=%d] [PipelineNum=%d] [MaxRequests=%d] [RequestSleepTime=%.1f] [LogLevel=%d]" \
			% (sys.argv[0], TargetClients, RequestPerConnection, PipelineNum, MaxRequests, RequestSleepTime, log.getLogLevel())
		sys.exit()

	paramIndex = 1
	HostIP = sys.argv[paramIndex]

	paramIndex += 1
	Port = int(sys.argv[paramIndex])

	paramIndex += 1
	if len(sys.argv) > paramIndex:
		args = sys.argv[paramIndex].split("/")

		if len(args) == 5 :
			TargetClients = -1
			MinClients = int(args[0])
			MaxClients = int(args[1])
			StepClients = int(args[2])
			IntervalTime = int(args[3])
			LastTime = int(args[4])

		else:
			TargetClients = int(sys.argv[paramIndex])

	paramIndex += 1
	if len(sys.argv) > paramIndex:
		RequestPerConnection = int(sys.argv[paramIndex])

	paramIndex += 1
	if len(sys.argv) > paramIndex:
		PipelineNum = int(sys.argv[paramIndex])

	paramIndex += 1
	if len(sys.argv) > paramIndex:
		MaxRequests = int(sys.argv[paramIndex])

	paramIndex += 1
	if len(sys.argv) > paramIndex:
		RequestSleepTime = float(sys.argv[paramIndex])

	paramIndex += 1
	if len(sys.argv) > paramIndex:
		log.setLogLevel(int(sys.argv[paramIndex]))

#----------------------------------------------------------------------------
def getConnectionCount(Clients):
	count = 0
	for c in Clients:
		if c.isConnected:
			count += 1

	return count

#----------------------------------------------------------------------------
def getConnectionFailCount(Clients):
	count = 0
	for c in Clients:
		count += c.countConnectionFail

	return count

#----------------------------------------------------------------------------
def getConnectionSuccessCount(Clients):
	count = 0
	for c in Clients:
		count += c.countConnectionSuccess

	return count

#----------------------------------------------------------------------------
def getSelectTimeoutCount(Clients):
	count = 0
	for c in Clients:
		count += c.countSelectTimeout

	return count

#----------------------------------------------------------------------------
def getRecvDataSize(Clients):
	size = 0
	for c in Clients:
		size += c.recvDataSize

	return size


Clients = []

def startClients(reactor, runTimeInfo, UrlList, targetClients, timeout, intervalTime=0, stepNums=0):
	global SummaryList
	global RequestPerConnection
	global IsRun
	global MaxRequests
	global RequestSleepTime
	global PipelineNum
	global Clients
	
	moreClients = targetClients - len(Clients)
	if stepNums != 0:
		if len(Clients) == 0:
			moreClients = MinClients
		elif moreClients > stepNums:
			moreClients = stepNums;
		
	prevSize = len(Clients)
	for i in range(0, moreClients):
		MIN = "0107777%04d" % (i + prevSize)
		c = eh.eh(reactor, runTimeInfo, RequestPerConnection, UrlList, MaxRequests, HostIP, Port, RequestSleepTime, MIN, PipelineNum)
		c.start()
		Clients.append(c)
	print "%d Clients started" % moreClients
	return moreClients
	
import datetime
#----------------------------------------------------------------------------
def startRequest(reactor, runTimeInfo, UrlList, targetClients, timeout, intervalTime=0, stepNums=0):
	global SummaryList
	global RequestPerConnection
	global IsRun
	global MaxRequests
	global RequestSleepTime
	global PipelineNum
	global Clients
	
	IsRun = True
	
	startClients(reactor, runTimeInfo, UrlList, targetClients, timeout, intervalTime, stepNums)
	
	programTimeout = time.time() + timeout
	programStartTime = time.time()
	
	# event loop
	oldRequestNum = 0
	
	steptimeout = 0
	
	while IsRun:
		newRequestNum = len(runTimeInfo)
		diffRequestNum = newRequestNum - oldRequestNum
		oldRequestNum = newRequestNum

		if log.getLogLevel() == 0:
			#d = time.localtime(time.time())
			#dstr =  "%02d:%02d:%02d" % (d[3], d[4], d[5])
			dstr = str(datetime.datetime.now())
			
			if timeout > 0:
				leftTime = programTimeout - time.time()
				print dstr, "Clients: %d/%d, Request: %d(+%3d), ConFail: %d, Timeout: %d, Recv: %.1fM, LeftTime: %dsec" % \
					(getConnectionCount(Clients), len(Clients), newRequestNum, diffRequestNum, getConnectionFailCount(Clients), getSelectTimeoutCount(Clients), getRecvDataSize(Clients)/1024.0/1024.0, leftTime)

			elif MaxRequests > 0:
				print dstr, "Clients: %d/%d, Request: %d(+%3d)/%d, ConFail: %d, Timeout: %d, Recv: %.1fM" % \
					(getConnectionCount(Clients), len(Clients), newRequestNum, diffRequestNum, MaxRequests, getConnectionFailCount(Clients), getSelectTimeoutCount(Clients), getRecvDataSize(Clients)/1024.0/1024.0)

			else:
				print dstr, "Clients: %d/%d, Request: %d(+%3d), ConFail: %d, Timeout: %d, Recv: %.1fM" % \
					(getConnectionCount(Clients), len(Clients), newRequestNum, diffRequestNum, getConnectionFailCount(Clients), getSelectTimeoutCount(Clients), getRecvDataSize(Clients)/1024.0/1024.0)

		# 종료 조건 확인
		if MaxRequests > 0 and len(runTimeInfo) >= MaxRequests:
			IsRun = False
			break

		if timeout > 0 and programTimeout < time.time():
			break

		try:
			time.sleep(1)
		except:
			pass
		
		if stepNums != 0:
			steptimeout += 1
			if steptimeout >= intervalTime:
				steptimeout = 0
				newclients = startClients(reactor, runTimeInfo, UrlList, targetClients, timeout, intervalTime, stepNums)
				if newclients==0:
					intervalTime = LastTime
			

	for c in Clients:
		c.isRun = False

	programEndTime = time.time()
	programRunTime = programEndTime - programStartTime
	
	for c in Clients:
		c.join()


	strSummary = getSummary(runTimeInfo, programRunTime, getConnectionSuccessCount(Clients), targetClients, getConnectionFailCount(Clients), getSelectTimeoutCount(Clients))

	print "-----------------------------------------------------------------------"
	print strSummary

#	filename = "c%03d_sizeBasedTime.csv" % targetClients
#	saveSizeBasedTime(filename, runTimeInfo, strSummary)

#	filename = "c%03d_runTimeInfo.csv" % targetClients
#	saveRunTimeInfo(filename, runTimeInfo)

	stats = getSummaryStats(runTimeInfo, programRunTime, getConnectionSuccessCount(Clients), targetClients, getConnectionFailCount(Clients), getSelectTimeoutCount(Clients))
	return stats



#----------------------------------------------------------------------------
if __name__ == "__main__":
	import sys
	import time

	# 시그널 핸들러 등록
	signal.signal(signal.SIGINT, programStop)
	signal.signal(signal.SIGTERM, programStop)

	# 파라미터 파싱
	parseParam()

	# reactor 생성
	myreactor = reactor.reactorR()

	# 요청할 URL 리스트 읽어 오기
	UrlList = getUrlList()

	if TargetClients > 0:
		runTimeInfo = []
		startRequest(myreactor, runTimeInfo, UrlList, TargetClients, -1)

	else:
		sumRunTimeInfo = []
		sumStats= []
		runTimeInfo = []
		
		stats = startRequest(myreactor, runTimeInfo, UrlList, MaxClients, -1, IntervalTime, StepClients)
		sumStats.append(stats)
		saveSumStats(sumStats)

