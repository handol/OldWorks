
URLS = [ 
			"http://www.magicn.com/",
			"http://www.magicn.com/index_n2.asp",
			]


	
def doit(fname):
	fp = open(fname, 'r')

	phonelist = {}
	for line in fp:
		flds = line.split()
		if len(flds) < 14:
			continue
		try:
			brcnt = int(flds[11])
			respcode = int(flds[12])
		except:
			#print flds[11]
			continue

		#if (flds[13]== URLS[0] and brcnt==1) or (flds[13]== URLS[1] and brcnt==3):
		if (brcnt == 1 and respcode==302) or brcnt == 2:
			print flds[2], brcnt, respcode, flds[10],  flds[13]

import sys
doit(sys.argv[1])

