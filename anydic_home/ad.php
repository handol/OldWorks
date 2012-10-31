<?php

	/*
	print "--- POST\n";
	foreach ($_POST as $line) {
		print "$line\n";
	}
	*/

	/*
	print "--- GET\n";
	*/

	$str = "";
	foreach ($_GET as $key => $value) {
		//print "$key = $value\n";
		$str .= "$key=$value&";
	}


	exec("export QUERY_STRING='$str'; /var/www/html/ad.dic", $res) or die ("exec failed");
	$n = 0;
	foreach ($res as $line) {
		if ($n > 0)
			print "$line\n";
		$n = $n + 1;
	}


?>
