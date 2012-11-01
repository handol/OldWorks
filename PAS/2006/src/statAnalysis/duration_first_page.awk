function getTimestamp(date, time)
{
	year = substr(date, 1, 4)
	month = substr(date, 6, 2)
	day = substr(date, 9, 2)

	hour = substr(time, 1, 2)
	min = substr(time, 4, 2)
	sec = substr(time, 7, 2)

	return mktime(year " " month " " day " " hour " " min " " sec)
}

{ 
	OFMT = "%.4f"

	prevMDN = 0

	do
	{
		if(FN == 10)
		{
			mdn = $1
			date = $2
			time = $3
			counter = $4
			santa = $5
			guide = $6
			cp = $7
			phone = $8
			rescode = $9
			url = $10
		}
		else if(FN == 11)
		{
			mdn = $1
			date = $2
			time = $3
			counter = $4
			santa = $5
			guide = $6
			cp = $7
			phone = $8
			phone_start = $9
			rescode = $10
			url = $11
		}

		if(prevMDN != mdn)
		{
			magic_start = 0
			magic_end = 0
			wipi_start = 0
			wipi_end = 0
			visible_start = 0
			visible_end = 0
		}
		else
		{
			if(prevCounter + 1 != counter)
			{
				magic_start = 0
				magic_end = 0
				wipi_start = 0
				wipi_end = 0
				visible_start = 0
				visible_end = 0
			}
		}
		
		if(counter == "001")
		{
			magic_start = 0
			magic_end = 0
			wipi_start = 0
			wipi_end = 0
			visible_start = getTimestamp(date, time) + 0.5 - phone
			visible_end = 0

			if(url=="http://www.magicn.com/")
			{
				magic_start = getTimestamp(date, time) + 0.5 - phone
				magic_guide = "F"
			}
			else if(url=="http://ktfwipidc.magicn.com:80/default.asp")
			{
				wipi_start = getTimestamp(date, time) + 0.5 - phone
				wipi_guide = "F"
			}
		}
		else
		{
			if(visible_start > 0 && rescode == "200")
			{
				visible_end = getTimestamp(date, time) + 0.5  
				visible_diff = visible_end - visible_start
				visible_sum += visible_diff
				visible_count++

				visible_start = 0
				visible_end = 0
			}
		}

		if(magic_start > 0 && prevMDN == mdn)
		{
			if(match(url, "http://www.magicn.com/guide.asp") == 1)
			{
				magic_guide = "T"
			}	
			else if( url == "http://www.magicn.com/R2/default/23407.asp" && counter != "001")
			{
				magic_end = getTimestamp(date, time) + 0.5

				duration = magic_end - magic_start

				if(duration >= 10 && magic_guide == "F")
					printf "%s %8.4f %s %s\n", mdn,  duration, magic_guide, url

				if(magic_guide == "T")
				{
					sum_magic_guide += duration
					count_magic_guide++;
				}
				else
				{
					sum_magic_nonguide += duration
					count_magic_nonguide++;
				}

				magic_start = 0
				magic_end = 0
			}
		}
		else if(wipi_start > 0 && prevMDN == mdn)
		{
			if(match(url, "http://ktfwipidc.magicn.com/guide.asp") == 1)
			{
				wipi_guide = "T"
			}
			else if( (url == "http://ktfwipidc.magicn.com/multipack/default/3000.asp" || url == "http://ktfwipidc.magicn.com:80/multipack/default/3000.asp") && counter != "001")
			{
				wipi_end = getTimestamp(date, time) + 0.5

				duration = wipi_end - wipi_start

				if(duration >= 10 && wipi_guide == "F")
					printf "%s %8.4f %s %s\n", mdn,  duration, wipi_guide, url

				if(wipi_guide == "T")
				{
					sum_wipi_guide += duration
					count_wipi_guide++;
				}
				else
				{
					sum_wipi_nonguide += duration
					count_wipi_nonguide++;
				}

				wipi_start = 0
				wipi_end = 0
			}
		}

		prevMDN = mdn
		prevCounter = counter
	} 
	while(getline > 0)

	if(count_magic_nonguide > 0)
		print "Magic : count["count_magic_nonguide"] average["sum_magic_nonguide / count_magic_nonguide"]"

	if(count_magic_guide > 0)
		print "Magic Guide : count["count_magic_guide"] average["sum_magic_guide / count_magic_guide"]"

	if(count_wipi_nonguide > 0)	
		print "Wipi : count["count_wipi_nonguide"] average["sum_wipi_nonguide / count_wipi_nonguide"]"

	if(count_wipi_guide > 0)
		print "Wipi Guide : count["count_wipi_guide"] average["sum_wipi_guide / count_wipi_guide"]"
	
	if(visible_count > 0)
		print "Visible : count["visible_count"] average["visible_sum/visible_count"]"
}
