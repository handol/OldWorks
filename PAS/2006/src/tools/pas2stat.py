#!/usr/bin/env python
import sys
import random

def get_cp_and_svc(cp_svc):
	if cp_svc == "(N/A)":
		return "N/A", "N/A"
		#	cpname = "N/A"
		#	svccode = "N/A"
	else:
		cp_svc = cp_svc[1:-1]
		s = cp_svc.split(';')
		if len(s) < 2:
			return "N/A", "N/A"

		cpname = s[0].split('=')
		svccode = s[1].split('=')

		if len(cpname) < 2:
			return "N/A", "N/A"
		if len(svccode) < 2:
			return "N/A", "N/A"

		return cpname[1], svccode[1]



## 00:01:30 --> 90
def get_sec_from_timestr(timestr):
	flds = timestr.split(':')
	flds = map(int, flds)
	return flds[0]*3600 + flds[1]*60 + flds[2]


def get_day_hour(day, hour):
	day = day.replace('/', '')
	hour = hour.replace(':', '')
	return day + hour
	
## return pasidr line from passtat line.
# f = statflds
mdn_dict = {}
def conv_pas2stat(line):
	f = line[:40].split()
	if f[3][0] == '-':
		up_down = 0  # up
	else:
		up_down = 1  # down

	if f[2]=='[SSL]':
		continue

	bytes = int(f[6]) + int(f[7])

	#rescode = f[11]
	#url = f[12]
	day = f[0]
	hour = f[1]
	mdn =f[2]
	imsi = f[3]
	cprespmsec = f[4]
	trandiffsec = int(f[5])
	reqbytes = int(f[6])
	respbytes = int(f[7])
	model = f[8]
	browser = f[9]
	cp_and_svc = f[10]
	httpcode = int(f[11])
	url = f[12]
	
	cpname, svccode = get_cp_and_svc(cp_and_svc) # CPname, Svc Code		
	if len(cpname) > 20:
		cpname = cpanme[:20]
	if len(svccode) > 20:
		svccode = svccode[:20]
	
	# 2006/10/10 12:12:55 --> 20061010121255
	timeval = get_day_hour(day, hour)
	
	phoneIpAddr = "10.%d.%d.%d" % (random.randint(53,220), random.randint(9,220), random.randint(5,220));
	wagle_ipaddr = "211.43.204.190"
	wagle_port = 80
	
	linelength = 125+9+68+len(url);
	linelength += 18
	idr = "%04d A %-11s %14s %09d %09d %-14s %-15s %-15s %-15s %-6d %-15s %-14s %-15s %-6d %-20s %-20s %s\n" % \
		(
		linelength,
		mdn,
		timeval,
		trandiffsec,
		reqbytes + respbytes,
		timeval,
		model,
		browser,
		phoneIpAddr,
		httpcode,
		pashost_name,
		timeval,
		wagle_ipaddr,
		wagle_port,
		cpname,
		svccode,
		url
		)
	
	if linelength != len(idr)-1:
		print "linelength WRONG %d -- %d" % (linelength, len(idr)-1)
	return idr


## idr  FILE NAME
def get_idr_fname(day, hour):
	day = day.replace('/', '')
	hour = hour.replace(':', '')
	minute = int(hour[2:4])
	minute = (minute / 5) * 5
	hour = hour[:2]
	hour = hour + "%02d" % minute
	fname = "k_n_pasidr.%s%s.log.pasgw1" % ( day, hour)
	return fname


#

def pas2stat(paslogfile, starttime="00:00:00", endtime="23:59:59"):
	try:
		fd = open(paslogfile, 'r')
	except:
		print "file NOT found:", fname
		return
	
	passtatfile = paslogfile.replace('k_pas', 're_passtat')
	print "writes %s" % passtatfile
	out = open(passtatfile, 'w')
	out.write(idr)

	for line in fd:
		timeval = line[11:19]
		if timeval >= starttime: break
		#print timeval
		#break

	minute = -1
	outf = ""

	for line in fd:
	

		timeval = line[11:19]
		if timeval > endtime: break

		flds = line.split()

		idr = conv_stat2idr("pasgw1", flds)


if __name__=="__main__":
	if len(sys.argv) > 2:
		stat2idr(sys.argv[1], sys.argv[2], sys.argv[3])
	else:
		#print "usage: passtat_file start_time end_time"
		stat2idr(sys.argv[1])
		


