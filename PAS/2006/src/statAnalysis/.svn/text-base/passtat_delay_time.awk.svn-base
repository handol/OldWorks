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
		browser = $10
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

				guide_delay_count[msmodel, int(diff)]++
				guide_delay_count[browser, int(diff)]++
			}
			else
			{
				non_guide_sum[service] += diff
				non_guide_count[service]++

				non_guide_delay_count[msmodel, int(diff)]++
				non_guide_delay_count[browser, int(diff)]++
			}

			delay_browser[browser]++
			delay_count[browser, int(diff)]++

			delay_model[msmodel]++
			delay_count[msmodel, int(diff)]++
		}

		prevMDN = mdn
		prevPhoneStart = phone_start

	}
	while(getline > 0)

	print "--------------------------[MSMODEL]--------------------------------------------"

	for(m in delay_model)
	{
		printf "%15s : ", m
		for(i=0; i < 30; i++)
		{
			if(guide_delay_count[m, i] == 0)
				printf "     "
			else
				printf "%5d", guide_delay_count[m, i]

			if(i == 29)
				printf "\n"
			else
				printf ","
		}
	}

	print "--------------------------[BROWSER]--------------------------------------------"

	for(m in delay_browser)
	{
		printf "%15s : ", m
		for(i=0; i < 30; i++)
		{
			if(guide_delay_count[m, i] == 0)
				printf "     "
			else
				printf "%5d", guide_delay_count[m, i]

			if(i == 29)
				printf "\n"
			else
				printf ","
		}
	}

	print "--------------------------[TOTAL]----------------------------------------------"

	printf "%15s : ", "Total"
	for(i=0; i < 30; i++)
	{
		total_count = 0

		for(m in delay_browser)
			total_count += guide_delay_count[m, i] + non_guide_delay_count[m, i]

		printf "%5d", total_count

		if(i == 29)
			printf "\n"
		else
			printf ","
	}
}

