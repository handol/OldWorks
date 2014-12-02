#!/usr/local/bin/perl
#
# Convert UTF8 encoding to Binary Unicode encoding
#
# In 16bit code area...
#
# UTF-8					Unicode
# 1110xxxx 10yyyyzz 10wwwwww		xxxxyyyy zzwwwwww
#

while (<STDIN>) {

	chop;
	$line = $_;

	@bytes = split(//, $line);

	while (@bytes) {
		$a = ord(shift(@bytes));

		if (($a & 0x80) == 0) {
			printf "%c", $a;
		}
		elsif (($a & 0xe0) == 0xe0) {
			$b = ord(shift(@bytes));
			$c = ord(shift(@bytes));

			printf "%c%c", (($a & 0x0f) << 4) | (($b & 0x3f) >> 2), (($b & 0x03) << 6) | ($c & 0x3f);
		}
	}

	print "\n";
}
