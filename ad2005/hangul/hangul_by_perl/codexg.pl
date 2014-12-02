#
# Code Conversion Functions
#

require 'ks2code.pl';
require 'code2roman.pl';

sub InputToCode
{
	my ($input, $mode) = @_;
	my ($output);

	if ( $mode =~ /^UNI/ )
	{
		# Check valid code range...

		if ($input < 44032 || $input > 55203)
		{
			$output = "";
		}
		else
		{
			use integer;

			$cho = ($input - 44032) / 588 + 1;
			$jung = (($input - 44032) % 588) / 28 + 1;
			$jong = ($input - 44032) % 28;

			if ($cho < 10) { $cho = "0".$cho; };
			if ($jung < 10) { $jung = "0".$jung; };
			if ($jong < 10) { $jong = "0".$jong; };

			$output = "i".$cho."j".$jung."k".$jong;
		}
	}
	elsif ( $mode =~ /^KSC/ )
	{
		# Check valid code range...

		if ($input < 45216 || $input > 51455)
		{
			$output = "";
		}
		else
		{
			$output = $ks2code{ $input };
		}
	}
	else
	{
		print "There is no mached character set!.\n";
		$ouput = "";
	}

	return $output;
}

sub CodeToRoman
{
	my ($input) = @_;

	my ($output, $unit, $offset3, $inputlen);

	# 표기상예외처리..
	$input =~ s/k08i06/k08i26/g; #ㄹㄹ -> ll

	$inputlen = length $input;
	$offset = 0;

	while ( $inputlen != $offset3 )
	{
		$unit = substr $input, $offset3, 3;
		$output .= $code2roman{ $unit };
		$offset3 += 3;
	}

	return $output;
}

sub CodeToHan
{
	my ($input, $mode) = @_;

	my ($output, $intpulen, $offset9, $unit, $code, $unicode);

	$inputlen = length $input;

	if ( $mode =~ /^UNI/ )
	{
		$offset9 = 0;

		while( $inputlen != $offset9 )
		{
			$letter = substr $input, $offset9, 9;

			if ($letter eq "i99j99k99")
			{
				$offset9 += 9;
				next;
			}

			use integer;

			$unicode = 0;

			$unit = substr $letter, 1, 2;
			$unicode += ($unit - 1) * 588;

			$unit = substr $letter, 4, 2;
			$unicode += ($unit - 1) * 28;

			$unit = substr $letter, 7, 2;
			$unicode += $unit;

			$unicode += 44032;
			$output .= sprintf "%c%c", ( $unicode >> 8 ), ( $unicode & 0x00ff );

			$offset9 += 9;
		}
	}
	elsif ( $mode =~ /^KSC/ )
	{
		$offset9 = 0;

		while ( $inputlen != $offset9 )
		{
			$unit = substr $input, $offset9, 9;

			if ($unit eq "i99j99k99")
			{
				$offset9 += 9;
				next;
			}

			$code = $code2ks{ $unit };

			if ( !defined($code) ) 
			{ 
				my $first = substr $unit , 0 , 6;
				my $last = substr $unit , 7 , 2;
				$code=$code2ks { $first."k00" };
				my $hehe=$chong[$last];
				$offset9 += 9;

				$output .= sprintf "%c%c%s", ( $code >> 8 ), ( $code & 0x00FF ), $hehe;
			}
			else
			{

				$offset9 += 9;

				$output .= sprintf "%c%c", ( $code >> 8 ), ( $code & 0x00FF );
			}
		}
	}
	else
	{
		print "There is no mached character set!.\n";
		return;
	}

	return $output;
}
