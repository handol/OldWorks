{ 
	OFMT = "%.4f"

	do
	{
		# santa
		if ($7 > 0)
		{
			santa_lines += 1
			santa_sum += $7
		} 

		# guide
		if ($8 > 0)
		{
			guide_lines += 1
			guide_sum += $8
		} 

		# cp
		if ($9 > 0)
		{
			cp_lines += 1
			cp_sum += $9
		} 

		# phone
		if ($10 > 0)
		{
			phone_lines += 1
			phone_sum += $10
		} 
	} 
	while(getline > 0)

	if(santa_lines > 0)
		print "santa : sum is", santa_sum, ", count is", santa_lines, ", average is", santa_sum/santa_lines

	if(guide_lines > 0)
		print "guide : sum is", guide_sum, ", count is", guide_lines, ", average is", guide_sum/guide_lines

	if(cp_lines > 0)
		print "CP    : sum is", cp_sum, ", count is", cp_lines, ", average is", cp_sum/cp_lines

	if(phone_lines > 0)
		print "phone : sum is", phone_sum, ", count is", phone_lines, ", average is", phone_sum/phone_lines
}
