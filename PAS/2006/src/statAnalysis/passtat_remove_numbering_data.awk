{ 
	do
	{
		printf "%s %s %-11s %-15s %s %3d %5d %5d %12s %10s %s %3d %s\n", $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17
	}
	while(getline > 0)
}
