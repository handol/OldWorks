import sys

# implement divide operation without using built-in divide operator
def divAndMod_slow(y,x, debug=0):
	r = 0
	while y >= x:
		r += 1
		y -= x
	return r,y
	

# implement divide operation without using built-in divide operator
def divAndMod(y,x, debug=0):

	## find the highest position of positive bit of the ratio
	pos = -1
	while y >= x:
		pos += 1
		x <<= 1
	x >>= 1
	if debug: 
		print "y=%d, x=%d, pos=%d" % (y,x,pos)
	
	if pos == -1:
		return 0, y


	r = 0
	while pos >= 0:
		if y >= x:
			r += (1 << pos)
			if debug: print "y=%d, x=%d, r=%d, pos=%d" % (y,x,r,pos)
			y -= x
		else:
			if debug: print "y=%d, x=%d, r=%d, pos=%d" % (y,x,r,pos)
		
		x >>= 1
		pos -= 1

	return r, y


if __name__ =="__main__":
	if len(sys.argv) == 3:
		y = int(sys.argv[1])
		x = int(sys.argv[2])

		print "Fast Version ...."
		res = divAndMod( y, x, debug=1)
		print "%d = %d * %d + %d" % (y, x, res[0], res[1])
		msg = "print %d * %d + %d" % (x, res[0], res[1])
		exec msg

		print "Slow Version ...."
		res = divAndMod_slow( y, x)
		print "%d = %d * %d + %d" % (y, x, res[0], res[1])
		
