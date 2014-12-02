require("rules1.pl");
require("rules2.pl");
require("namerule.pl");

sub Rules
{
	my($input, $mode) = @_;

	#print $input;


	$result = &rule_hyphen($input, $mode);

	if ($mode != 2) {
		$result = &rule_ex($result);
		$result = &rule_ki_imbibition($result);
		$result = &rule_ki($result);
	}

	$result = &rule_kk($result);

	#print $result;

	return $result;
}
