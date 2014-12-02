#!/usr/local/bin/perl
#
# Main Processing Module
#
# Usage: kr2roman.pl [filename] [-c[KSC|UNI]]
#

# Default Parameters
$Code = KSC;	# KSC for KSC, UNI for UNICODE
$Filename = "";	# filename on file input mode
$Mode = 1;	# 1 for Normal, 2 for Person Name, 3 for Region Name

# Argument process
foreach (@ARGV) {
	ARGSWITCH: {
		# for code selection
		if (/^-cKSC/) { $Code=KSC; last ARGSWITCH; }
		if (/^-cUNI/) { $Code=UNI; last ARGSWITCH; }

		# for mode selection
		if (/^-mNORM/) { $Mode=1; last ARGSWITCH; }
		if (/^-mPNAME/) { $Mode=2; last ARGSWITCH; }
		if (/^-mRNAME/) { $Mode=3; last ARGSWITCH; }

		# for help
		if (/^-h|-help/) { &PrintUsage(); exit; }

		# for unknown option
		if (/^-.*/) { &PrintUsage(); exit; }

		# else we assume it's filename
		$Filename = $_;
	}
}

# include necessary modules
require("codexg.pl");
require("rules.pl");

# input multiplexing
if ($Filename ne "") {
	open(FHANDLE, $Filename)
		or die " Can't open file $Filename...\n";

	$InPipe = FHANDLE;
}
else {
	$InPipe = STDIN;
	print " kr2roman.pl - Romanize Korean Text\n";
	print " Enter Strings to Romanize...\n";
	print " To Quit, press Ctrl-D (in Unix) or Ctrl-Z (in Windogs)\n\n";
}


# Main Processing loop
while (<$InPipe>) {

	$word = '';
	$output = '';
	$output2 = '';

	chop;
	if (length($_) == 0) { next; };

	$_ .= " ";

	foreach (&mkchars($_)) {
		if ($_ > 0xFF) {
			$word .= &InputToCode($_, $Code);
		}
		else {
			$a = $_;

			if ($Mode == 2) {
				($lname, $fname) = &SplitName($word);

				$lname = &Rules($lname, $Mode);
				$fname = &Rules($fname, $Mode);

				$str = sprintf "%s %s%c", ucfirst(&CodeToRoman($lname)), ucfirst(&CodeToRoman($fname)), $a;
				$output .= $str;

				$str = sprintf "%s%c", &CodeToHan($word, $Code), $a;
			}
			else {
				$word = &Rules($word, $Mode);

				if ($Mode != 1) {
					$str = sprintf "%s%c", ucfirst(&CodeToRoman($word)), $a;
				}
				else {
					$str = sprintf "%s%c", &CodeToRoman($word), $a;
				}
				$output .= $str;

				$str = sprintf "%s%c", &CodeToHan($word, $Code), $a;
			}

			$output2 .= $str;

			$word = '';
		}
	}

	chop ($output);
	chop ($output2);

	print $output;
	print "(".$output2.")\n";
}


# ----------------------------
# Definitions for sub routines
# ----------------------------

# Display Help message...
sub PrintUsage {
	print " kr2roman.pl - Romanize Korean Text version 0.9\n";
	print " Usage: ./kr2roman.pl [filename] [-c[KSC|UNI]] [-m[NORM|PNAME|RNAME]]\n\n";
	print " Options\n";
	print "	filename	Text filename for conversion\n";
	print "	-c[KSC|UNI]	Code selection. KSC for KSC5601 and UNI for Unicode.\n";
	print "	-m[NORM|PNAME|RNAME]	Mode selection\n";
	print "			NORM	: Default. Normal mode.\n";
	print "			PNAME	: Personal name mode.\n";
	print "			RNAME	: Regional name mode.\n";
	print "\n";
}

sub mkchars {
    local($line) = shift;
    local($i, @bytes, @out);
 

    @bytes = split(//, $line);
    while (@bytes) {
        $b = ord(shift(@bytes));
        if (($b & 0x80) != 0) {
            $b = ($b << 8) | ord(shift(@bytes));
        }

        push(@out, $b);
    }
    return @out;
}
