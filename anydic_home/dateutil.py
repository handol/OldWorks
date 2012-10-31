import time
import datetime

"""
%Y Year with century as a decimal number. 

%m Month as a decimal number [01,12].  
%d Day of the month as a decimal number [01,31].  
%H Hour (24-hour clock) as a decimal number [00,23]. 
%M Minute as a decimal number [00,59]. 

%w Weekday as a decimal number [0(Sunday),6].  
%W Week number of the year (Monday as the first day of the week) as a decimal number [00,53]. All days in a new year preceding the first Monday are considered to be in week 0. 
"""
def currday():
	#return time.strftime('%Y%m%d')
	tm = time.localtime()
	return "%d%02d%02d" % (tm.tm_year, tm.tm_mon, tm.tm_mday)

def currhour():
	#return time.strftime('%Y%m%d%H')
	tm = time.localtime()
	return "%d%02d%02d%02d" % (tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour)

def currh():
	#return time.strftime('%Y%m%d%H')
	tm = time.localtime()
	return "%02d" % (tm.tm_hour)
	
def daybefore(delta=1):
	today = datetime.date.today( )
	yester = today - datetime.timedelta(days=delta)
	return yester.strftime('%Y%m%d')

def hourbefore(delta=1):
	today = datetime.date.today( )
	yester = today - datetime.timedelta(hours=delta)
	return yester.strftime('%Y%m%d%H')
	

if __name__=="__main__":
	print currday(), currhour(), daybefore(), hourbefore()
