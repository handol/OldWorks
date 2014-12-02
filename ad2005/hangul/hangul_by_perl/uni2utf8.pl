#!/usr/local/bin/perl
#
# Convert Binary Unicode encoding to UTF8 encoding
#
# In 16bit code area...
#
# UTF-8					Unicode
# 1110xxxx 10yyyyzz 10wwwwww		xxxxyyyy zzwwwwww
#

while (<STDIN>) {

	$line = $_;

	@bytes = split(//, $line);

	while (@bytes) {
		$a = ord(shift(@bytes));

		if (($a & 0x80) == 0) {
			printf "%c", $a;
		}
		else {
			$b = ord(shift(@bytes));

			printf "%c%c%c", ($a >> 4) | 0xe0,
					(($a & 0x0f) << 2) | ($b >> 6) | 0x80,
					($b & 0x3f) | 0x80;
		}
	}

	print "\n";
}
