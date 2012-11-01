#!/usr/bin/env python
import sys
import os


def rename_it(idr_name):
	new_name = ""

	n = 0
	while n < len(idr_name) and not idr_name[n].isdigit():
		new_name += idr_name[n]
		n += 1

	dgt_str = ""
	while n < len(idr_name) and idr_name[n].isdigit():
		dgt_str += idr_name[n]
		n += 1

	old_minute = minute = int(dgt_str[-2:])
	if minute % 10 == 0:
		minute = (minute / 10) * 10 + 1

	dgt_str = dgt_str[:-2] + "%02d" % minute

	new_name += dgt_str


	while n < len(idr_name) and not idr_name[n].isdigit():
		new_name += idr_name[n]
		n += 1

	print "%d --> %d" % (old_minute, minute)

	if old_minute != minute:
		print "%s --> %s" % (idr_name, new_name)
		cmd = "mv %s %s" % (idr_name, new_name)
		os.system(cmd)

	return new_name





if __name__=="__main__":
	if len(sys.argv) == 2:
		rename_it(sys.argv[1])
		


