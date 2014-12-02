require 'joeo.pl';

sub rule_ki_imbibition
{
#초성
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19
#ㄱ ㄲ ㄴ ㄷ ㄸ ㄹ ㅁ ㅂ ㅃ ㅅ ㅆ ㅇ ㅈ ㅉ ㅊ ㅋ ㅌ ㅍ ㅎ

#중성
# 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21
# ㅏ ㅐ ㅑ ㅒ ㅓ ㅔ ㅕ ㅖ ㅗ ㅘ ㅙ ㅚ ㅛ ㅜ ㅝ ㅞ ㅟ ㅠ ㅡ ㅢ ㅣ

#종성
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23
#ㄱ ㄲ ㄳ ㄴ ㄵ ㄶ ㄷ ㄹ ㄺ ㄻ ㄼ ㄽ ㄾ ㄿ ㅀ ㅁ ㅂ ㅄ ㅅ ㅆ ㅇ ㅈ ㅊ 
#24 25 26 27
#ㅋ ㅌ ㅍ ㅎ
	$merong=$_;
	$_=join(' ',@_);

# rule 30 사이시옷 뒤에 자음이 올때

	s/k19i01/k00i02/g; # 냇가 -> 내까
	s/k19i04/k00i05/g; # 콧등 -> 코ㄷ 등
	s/k19i08/k00i09/g; # 깃발 -> 기빨
	s/k19i10/k00i11/g; # 햇살 -> 해쌀
	s/k19i13/k00i14/g; # 뱃전 -> 배쩐


#s/k19i12j21/k04i03j21/g; # 깻잎 ->  깬닙

# rule 17
#s/k07i12j21/k00i13j21/g; # 굳이 -> 구지
#s/k25i12j21/k00i15j21/g; # 밭이 -> 바치
#s/k13i12j21/k08i15j21/g; # 벼훑이 -> 벼훌치
#
#
#s/k07i19j21/k00i15j21/g; # 굳히다 -> 구치다

# rule 18
	s/k01(i0[37])/k21$1/g;  # 먹는 -> 멍는
	s/k02(i0[37])/k21$1/g;  # 깎는 -> 깡는
	s/k03(i0[37])/k21$1/g;  # 몫몫이 -> 몽목씨
	s/k09(i0[37])/k21$1/g;  # 긁는 -> 긍는

	s/k07(i0[37])/k04$1/g;  # 닫는 -> 단는
	s/k19(i0[37])/k04$1/g;  # 짓는 -> 진는
	s/k20(i0[37])/k04$1/g;  # 있는 -> 인는
	s/k22(i0[37])/k04$1/g;  # 젖멍울 -> 전멍울
	s/k23(i0[37])/k04$1/g;  # 쫓는 -> 쫀는
	s/k25(i0[37])/k04$1/g;  # 붙는 -> 분는
	s/k27(i0[37])/k04$1/g;  # 놓는 -> 논는

	s/k17(i0[37])/k16$1/g;  # 밥물 -> 밤물
	s/k26(i0[37])/k16$1/g;  # 앞마당 -> 암마당
	s/k11(i0[37])/k16$1/g;  # 밟는  -> 밤는
	s/k14(i0[37])/k16$1/g;  # 읊는 -> 음는
	s/k18(i0[37])/k16$1/g;  # 값매다 -> 감매다

# rule 19

	s/k16i06/k16i03/g; # 담력 -> 담녁
	s/k21i06/k21i03/g; # 강릉 -> 강능
	s/k01i06/k21i03/g; # 백리 -> 뱅니
	s/k17i06/k17i03/g; # 협력 -> 혐녁


#초성
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19
#ㄱ ㄲ ㄴ ㄷ ㄸ ㄹ ㅁ ㅂ ㅃ ㅅ ㅆ ㅇ ㅈ ㅉ ㅊ ㅋ ㅌ ㅍ ㅎ

#중성
# 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21
# ㅏ ㅐ ㅑ ㅒ ㅓ ㅔ ㅕ ㅖ ㅗ ㅘ ㅙ ㅚ ㅛ ㅜ ㅝ ㅞ ㅟ ㅠ ㅡ ㅢ ㅣ

#종성
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23
#ㄱ ㄲ ㄳ ㄴ ㄵ ㄶ ㄷ ㄹ ㄺ ㄻ ㄼ ㄽ ㄾ ㄿ ㅀ ㅁ ㅂ ㅄ ㅅ ㅆ ㅇ ㅈ ㅊ 
#24 25 26 27
#ㅋ ㅌ ㅍ ㅎ

# rule 20의 예외
	s/i12j20k00i01j07k04i06j01k04/i12j20k00i01j07k04i03j01k04/g; # 의견란
	s/i12j21k16i13j21k04i06j01k04/i12j21k16i13j21k04i03j01k04/g; # 임진란
	s/i10j02k21i10j01k04i06j03k21/i10j02k21i10j01k04i03j03k21/g; # 생산량
	s/i01j07k08i04j01k04i06j07k01/i01j07k08i05j01k04i03j07k01/g; # 결단력
	s/i01j09k21i01j15k04i06j07k01/i01j09k21i02j15k04i03j07k01/g; # 공권력
	s/i04j09k21i12j15k04i06j07k21/i04j09k21i12j15k04i03j07k21/g; # 동원령
	s/i10j01k21i01j07k04i06j08k00/i10j01k21i01j07k04i03j08k00/g; # 상견례
	s/i19j12k21i04j01k04i06j09k00/i19j12k21i04j01k04i03j09k00/g; # 횡단로
	s/i12j21k00i12j15k04i06j09k04/i12j21k00i12j15k04i03j09k04/g; # 이원론
	s/i12j21k17i12j15k04i06j13k00/i12j21k00i08j15k04i03j13k00/g; # 입원료
	s/i01j14k00i01j19k04i06j18k00/i01j14k00i01j19k04i03j18k00/g; # 구근료

# rule 20

	s/k04i06/k08i06/g; # 난로 -> 날로
	s/k08i03/k08i06/g; # 칼날 -> 칼랄

	s/k15i03/k08i06/g; # 닳는 -> 달른
	s/k13i03/k08i06/g; # 핥네 -> 할레

	$nya=$_;
	$_=$merong;
	return $nya;

}


sub rule_ki
{
#초성
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19
#ㄱ ㄲ ㄴ ㄷ ㄸ ㄹ ㅁ ㅂ ㅃ ㅅ ㅆ ㅇ ㅈ ㅉ ㅊ ㅋ ㅌ ㅍ ㅎ

#중성
# 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21
# ㅏ ㅐ ㅑ ㅒ ㅓ ㅔ ㅕ ㅖ ㅗ ㅘ ㅙ ㅚ ㅛ ㅜ ㅝ ㅞ ㅟ ㅠ ㅡ ㅢ ㅣ

#종성
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23
#ㄱ ㄲ ㄳ ㄴ ㄵ ㄶ ㄷ ㄹ ㄺ ㄻ ㄼ ㄽ ㄾ ㄿ ㅀ ㅁ ㅂ ㅄ ㅅ ㅆ ㅇ ㅈ ㅊ 
#24 25 26 27
#ㅋ ㅌ ㅍ ㅎ
	my $merong=$_;
	$_=join(' ',@_);


	while(/(.*?k(07|25|13))(i12j21.*)/)
	{
		my $first=$1;
		my $second=$3;

		my $isin = $joeo { $second };

		if( defined($isin) )
		{
# rule 17 받침 ㄷ,ㅌ이 조사나 접미사의 모음 ㅣ 와 결합되는 경우
			if($first=~ /k13$/)
			{
				$first=~s/k13$/k08/g;
				$second=~s/^i12/i15/g;

			}
			elsif($first=~/k25$/)
			{
				$first=~s/k25$/k00/g;
				$second=~s/^i12/i15/g;
			}
			else
			{
				$first=~s/k07$/k00/g;
				$second=~s/^i12/i13/g;
			}
		}
		else
		{
# rule 17 받침 ㄷ,ㅌ이 모음 ㅣ 와 만났지만 조사나 접미사가 아닌경우
			if($first=~ /k13$/)
			{
				$first=~s/k13$/k08/g;
				$second=~s/^i12/i17/g;

			}
			elsif($first=~/k25$/)
			{
				$first=~s/k25$/k00/g;
				$second=~s/^i12/i17/g;
			}
			else
			{
				$first=~s/k07$/k00/g;
				$second=~s/^i12/i04/g;
			}
		}
		$_=$first.$second;
	
	}

	while(/(.*?k[1-2][0-9])(i12j(01|05|09|14|17|21|03|07|13|18).*)/ || /(.*?k0[1-9])(i12j(01|05|09|14|17|21|03|07|13|18).*)/)
	{
		my $first=$1;
		my $second=$2;

		my $isin = $joeo { $second };

		if( !defined($isin) )
		{

			if($second =~ /^(i12)(j21|j03|j07|j13|j18)/)
			{
#rule 29 자음종성뒤에 어미가 아닌 모음 ㅣ,ㅑ,ㅛ,ㅕ,ㅠ 등이 올때 ㄴ 첨가.. 
				$second=~s/^i12/i03/g;
				$_=rule_ki_imbibition($first.$second);
			}
			else
			{
# rule 15 받침 뒤에 모음 ㅏ,ㅓ,ㅗ,ㅜ,ㅟ들로 시작되는 실질 형태소가 올때 
#         받침을 대표음으로 바꾸어 연음
				$first=rule_ki_org($first);
				$first=rule_kk($first);
				$first=~ s/(k[0-2][0-9])$/k00/g;
				my $third=$1;
				
				$third=rule_ki_small($third);
	
				$second=~ s/^i12/$third/g;
				$_=$first.$second;
			}

		}
		else
		{
			$first=rule_ki_org($first);
			$first=~ s/(k[0-2][0-9])$/k00/g;
			my $third=$1;
			my $huhu=rule_ki_nya($third);

			$first=~ s/k00$/$huhu/g;

			$third=rule_ki_small($third);
			$second=~ s/^i12/$third/g;
			$_=$first.$second;
		}
	}
	$_=rule_ki_org($_);
	my $nya=$_;
	$_=$merong;
	return $nya;
}

sub rule_ki_nya
{
	my $merong=$_;
	$_=join(' ',@_);

# 연음에 의해서 받침이 바뀌는 것

	s/k01/k00/g; 
	s/k02/k00/g; 
	s/k03/k01/g; 
	s/k04/k00/g; 
	s/k05/k04/g; 
	s/k06/k00/g; 
	s/k07/k00/g;
	s/k08/k00/g;
	s/k09/k08/g;
	s/k10/k08/g;
	s/k11/k08/g;
	s/k12/k08/g;
	s/k13/k08/g;
	s/k14/k08/g;
	s/k15/k00/g;
	s/k16/k00/g;
	s/k17/k00/g;
	s/k18/k17/g;
	s/k19/k00/g;
	s/k20/k00/g;
	s/k22/k00/g;
	s/k23/k00/g;
	s/k24/k00/g;
	s/k25/k00/g;
	s/k26/k00/g;
	s/k27/k00/g;

	my $nya=$_;
	$_=$merong;
	return $nya;
}

sub rule_ki_small
{
	my $merong=$_;
	$_=join(' ',@_);

# 연음에 의해 받침이 뒷 모음에게 영향을 미치는것
	s/k00/i12/g;
	s/k01/i01/g; 
	s/k02/i02/g; 
	s/k03/i11/g; 
	s/k04/i03/g; 
	s/k05/i13/g; 
	s/k06/i03/g; 
	s/k07/i04/g;
	s/k08/i06/g;
	s/k09/i01/g;
	s/k10/i07/g;
	s/k11/i08/g;
	s/k12/i11/g;
	s/k13/i17/g;
	s/k14/i18/g;
	s/k15/i06/g;
	s/k16/i07/g;
	s/k17/i08/g;
	s/k18/i11/g;
	s/k19/i10/g;
	s/k20/i11/g;
	s/k22/i13/g;
	s/k23/i15/g;
	s/k24/i16/g;
	s/k25/i17/g;
	s/k26/i18/g;
	s/k27/i12/g;

	my $nya=$_;
	$_=$merong;
	return $nya;

}

sub rule_ki_org
{
	my $merong=$_;
	$_=join(' ',@_);

# rule 13 연음
	s/k01i12/k00i01/g; 
	s/k02i12/k00i02/g; 
	s/k03i12/k01i11/g; 
	s/k04i12/k00i03/g; 
	s/k05i12/k04i13/g; 
	s/k06i12/k00i03/g; 
	s/k07i12/k00i04/g;
	s/k08i12/k00i06/g;
	s/k09i12/k08i01/g;
	s/k10i12/k08i07/g;
	s/k11i12/k08i08/g;
	s/k12i12/k08i11/g;
	s/k13i12/k08i17/g;
	s/k14i12/k08i18/g;
	s/k15i12/k00i06/g;
	s/k16i12/k00i07/g;
	s/k17i12/k00i08/g;
	s/k18i12/k17i11/g;
	s/k19i12/k00i10/g;
	s/k20i12/k00i11/g;
	s/k22i12/k00i13/g;
	s/k23i12/k00i15/g;
	s/k24i12/k00i16/g;
	s/k25i12/k00i17/g;
	s/k26i12/k00i18/g;
	s/k27i12/k00i12/g;

	my $nya=$_;
	$_=$merong;
	return $nya;
}

sub rule_kk
{

#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23
#ㄱ ㄲ ㄳ ㄴ ㄵ ㄶ ㄷ ㄹ ㄺ ㄻ ㄼ ㄽ ㄾ ㄿ ㅀ ㅁ ㅂ ㅄ ㅅ ㅆ ㅇ ㅈ ㅊ 
#24 25 26 27
#ㅋ ㅌ ㅍ ㅎ

	$_=join(' ',@_);

# 아래 규칙은 전부 어말이거나 뒤에 자음이 올때의 발음
# 몇가지는 예외가 있다.
	s/k02/k01/g;
	s/k03/k01/g;
	s/k05/k04/g;
	s/k06/k04/g; # ㄶ은 ㅎ과 같은 원리
	s/k09/k01/g;
	s/k10/k16/g;
	s/k11/k08/g; # 단 밟과 넓은 예외
	s/k12/k08/g;
	s/k13/k08/g;
	s/k14/k17/g;
	s/k15/k08/g; # ㅀ도 ㅎ 과 같은 원리
	s/k18/k17/g;
	s/k19/k07/g;
	s/k20/k07/g;
	s/k22/k07/g;
	s/k23/k07/g;
	s/k24/k01/g;
	s/k25/k07/g;
	s/k26/k17/g;
	s/k27/k07/g; #ㅎ은 뒤에오는 자음에 의해 발음이 결정된다.
	return $_;
}
