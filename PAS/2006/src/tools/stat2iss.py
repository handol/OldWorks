#!/usr/bin/env python
import sys


## PAS ISS
"""
sprintf(str1, "%s^%f^%d^%s^%s^%s^%s^%s^%s",
user->mdn,
user->last_interval,
user->last_url_bytes,
user->msmodel,
cpname,
svccode,
user->last_resultcode,
user->szchannelinfo,
user->last_url);
"""

##  PAS STAT
"""
sprintf(str, "%-11s %-15s %.4f %4d %5d %5d %s %s (%s) %s %s",
user->mdn, tmp, user->last_interval, this_time-user->last_time,
user->last_url_request_bytes, user->last_url_response_bytes,
user->msmodel, user->browser, user->last_cpdata,
user->last_resultcode, user->last_url);
"""

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


## return pasiss line from passtat line.
# f = statflds
def conv_stat2iss(f):
	# f[2] == MDN
	# f[4] == response time

	#model = f[8]
	bytes = int(f[6]) + int(f[7])

	#rescode = f[11]
	#url = f[12]

	cpname, svccode = get_cp_and_svc(f[10]) # CPname, Svc Code		

	iss = "%s^%s^%s^%s^%d^%s^%s^%s^%s^N/A^%s\n" % \
		(f[0], f[1], f[2], f[4],
			bytes,
			f[8],
			cpname,
			svccode,
			f[11],
			f[12])
	return iss


## ISS  FILE NAME
def get_iss_fname(day, hour):
	day = day.replace('/', '')
	hour = hour.replace(':', '')
	fname = "k_pasiss.%s%s.log" % ( day, hour[:-2])
	return fname

## make pasiss.log from  passtat.log
# read passtat.log starting at the given time
# create and write pasiss.log by 5 minutes


def stat2iss(statfile, starttime, endtime):
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
		flds = line.split()
		if len(flds) != 13: 
			print "# %s" % line[:-1]
			continue

		timeval = line[11:19]
		if timeval > endtime: break

		iss = conv_stat2iss(flds)

		#cur_minutes = int(flds[1][:-2]
		cur_minutes = int(flds[1][3:5])
		if minute == -1 or (minute != cur_minutes and cur_minutes % 5 == 0):
			minute = cur_minutes	

			if outf != "":
				out.close()

			outf = get_iss_fname(flds[0], flds[1])
			print "writes %s" % outf
			out = open(outf, 'w')
		out.write(iss)




if __name__=="__main__":
	if len(sys.argv) > 2:
		stat2iss(sys.argv[1], sys.argv[2], sys.argv[3])
	else:
		print "usage: passtat_file start_time end_time"


