{
	if (NF == 13) 
	{ 
		printf "%-11s %s %s %03d %6.4f %6.4f %6.4f %6.4f 0 %3d %s\n", $3, $1, $2, $11, $7, $8, $9, $10, $12, $13 
	} 
	else if (NF == 14) 
	{ 
		printf "%-11s %s %s %03d %6.4f %6.4f %6.4f %6.4f %s %3d %s\n", $3, $1, $2, $12, $7, $8, $9, $10, $11, $13, $14 
	} 
}
