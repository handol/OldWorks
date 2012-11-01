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
		if(NF == 10)
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

			phone_start = getTimestamp(date, time) + 0.5 - phone
		}
		else if(NF == 11)
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

		if(prevMDN != mdn || prevCounter + 1 != counter)
		{
			start_time = 0
			end_time = 0
		}
		
		if(counter == "001")
		{
			#if(rescode == "302" && (url == "http://ktfwipidc.magicn.com/default.asp" || url == "http://ktfwipidc.magicn.com:80/default.asp"))
			if(rescode == "302")
			{
				start_time = phone_start
			}
		}
		else if(counter == "002" && start_time > 0)
		{
			end_time = phone_start

			diff = end_time - start_time

			if(match(url, "http://ktfwipidc.magicn.com/guide.asp") == 1 || match(url, "http://ktfwipidc.magicn.com:80/guide.asp") == 1)
			{
				guide_count++
				guide_sum += diff
			}
			else
			{
				non_guide_count++
				non_guide_sum += diff
			}
		}

		prevMDN = mdn
		prevCounter = counter

	}
	while(getline > 0)

	if(guide_count + non_guide_count > 0)
	{
		guide_percent = (guide_count / (guide_count + non_guide_count)) * 100
		non_guide_percent = (non_guide_count / (guide_count + non_guide_count)) * 100
	}
	
	print "Guide["guide_count", "guide_percent"%] Non-Guide["non_guide_count", "non_guide_percent"%]"
	print "Redirect to guide : Count[" guide_count "] Average[" guide_sum / guide_count "]"
	print "Redirect to menu  : Count[" non_guide_count "] Average[" non_guide_sum / non_guide_count "]"
	print "Redirect Total    : Count[" guide_count + non_guide_count "] Average[" (guide_sum + non_guide_sum) / (guide_count + non_guide_count) "]"
}
