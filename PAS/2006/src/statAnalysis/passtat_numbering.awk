{ 
	do
	{
		date = $1
		time = $2
		mdn = $3

		lineNum++

		if(mdn == "Anonymous")
			continue

		print $3 " " $1 " " $2 " " lineNum " " $0
	}
	while(getline > 0)
}
