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


def get_day_hour(day, hour):
	day = day.replace('/', '')
	hour = hour.replace(':', '')
	return day + hour
	
## return pasidr line from passtat line.
# f = statflds
def conv_stat2idr(pashost_name, f):
	# f[2] == MDN
	# f[4] == response time

	#model = f[8]
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

## make pasidr.log from  passtat.log
# read passtat.log starting at the given time
# create and write pasidr.log by 5 minutes


def stat2idr(statfile, starttime="00:00:00", endtime="23:59:59"):
	try:
		fd = open(statfile, 'r')
	except:
		print "file NOT found:", fname
		return

	for line in fd:
		timeval = line[11:19]
		if timeval >= starttime: break
		#print timeval
		#break

	minute = -1
	outf = ""

	for line in fd:
	
		if line.find("k_metamediawaglwagl100") == -1:
			continue
			
		flds = line.split()
		if len(flds) != 13: 
			print "# %s" % line[:-1]
			continue

		timeval = line[11:19]
		if timeval > endtime: break

		idr = conv_stat2idr("pasgw1", flds)

		#cur_minutes = int(flds[1][:-2]
		cur_minutes = int(flds[1][3:5])
		if minute == -1 or (minute != cur_minutes and cur_minutes % 10 == 0):
			minute = cur_minutes	

			if outf != "":
				out.close()

			outf = get_idr_fname(flds[0], flds[1])
			print "writes %s" % outf
			out = open(outf, 'w')
		out.write(idr)




if __name__=="__main__":
	if len(sys.argv) > 2:
		stat2idr(sys.argv[1], sys.argv[2], sys.argv[3])
	else:
		#print "usage: passtat_file start_time end_time"
		stat2idr(sys.argv[1])
		


