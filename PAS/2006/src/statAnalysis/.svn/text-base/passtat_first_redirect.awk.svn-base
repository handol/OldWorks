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


	do
	{
		date = $1
		time = $2
		mdn = $3
		cp = $5
		interval = $6
		msmodel = $9
		rescode = $12
		url = $13

		phone_start = getTimestamp(date, time) + 0.5 - cp
		counter++

		if(prevMDN != mdn || phone_start - prevPhoneStart >= 30)
		{
			start_time = 0
			end_time = 0
			counter = 1
			service = ""
		}
		
		if(counter == 1 && rescode == "302")
		{
			if(url == "http://www.magicn.com/")
			{
				start_time = phone_start
				service = "MagicN"
			}
			else if(url == "http://ktfwipidc.magicn.com/default.asp" || url == "http://ktfwipidc.magicn.com:80/default.asp")
			{
				start_time = phone_start
				service = "WIPI"
			}
		}
		else if(counter == 2 && start_time > 0)
		{
			end_time = phone_start

			diff = end_time - start_time
			diff_list[int(diff)]++

			if(match(url, "/guide.asp") > 0)
			{
				guide_sum[service] += diff
				guide_count[service]++
			}
			else
			{
				non_guide_sum[service] += diff
				non_guide_count[service]++
			}
		}

		prevMDN = mdn
		prevPhoneStart = phone_start

	}
	while(getline > 0)

	for(key in non_guide_count)
	{
		all_count["ALL"] += guide_count[key] + non_guide_count[key]
		all_sum["ALL"] += guide_sum[key] + non_guide_sum[key]
	}
	
	printf "%6s Redirect to       ALL : Count[%5d] Average[%f]\n", "ALL", all_count["ALL"], all_sum["ALL"]/all_count["ALL"]

	for(service in non_guide_count)
	{
		guide_percent[service] = (guide_count[service] / (guide_count[service] + non_guide_count[service])) * 100
		non_guide_percent[service] = (non_guide_count[service] / (guide_count[service] + non_guide_count[service])) * 100

		if(guide_count[service] > 0)
		{
			printf "%6s Redirect to     Guide : Count[%5d] Percent[%f%%] Average[%f]\n", service, guide_count[service], guide_percent[service], guide_sum[service]/guide_count[service]
			all_count[service] += guide_count[service]
			all_sum[service] += guide_sum[service]
		}

		if(non_guide_count[service] > 0)
		{
			printf "%6s Redirect to Non-Guide : Count[%5d] Percent[%f%%] Average[%f]\n", service, non_guide_count[service], non_guide_percent[service], non_guide_sum[service]/non_guide_count[service]
			all_count[service] += non_guide_count[service]
			all_sum[service] += non_guide_sum[service]
		}	

		printf "%6s Redirect to       ALL : Count[%5d] Percent[100%%] Average[%f]\n", service, all_count[service], all_sum[service]/all_count[service]
	}
}

