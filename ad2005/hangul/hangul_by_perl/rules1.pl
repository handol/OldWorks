require 'joeo.pl';

sub rule_ki_imbibition
{
#�ʼ�
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19
#�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��

#�߼�
# 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21
# �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��

#����
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23
#�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� 
#24 25 26 27
#�� �� �� ��
	$merong=$_;
	$_=join(' ',@_);

# rule 30 ���̽ÿ� �ڿ� ������ �ö�

	s/k19i01/k00i02/g; # ���� -> ����
	s/k19i04/k00i05/g; # ��� -> �ڤ� ��
	s/k19i08/k00i09/g; # ��� -> �⻡
	s/k19i10/k00i11/g; # �޻� -> �ؽ�
	s/k19i13/k00i14/g; # ���� -> ��¾


#s/k19i12j21/k04i03j21/g; # ���� ->  ����

# rule 17
#s/k07i12j21/k00i13j21/g; # ���� -> ����
#s/k25i12j21/k00i15j21/g; # ���� -> ��ġ
#s/k13i12j21/k08i15j21/g; # ������ -> ����ġ
#
#
#s/k07i19j21/k00i15j21/g; # ������ -> ��ġ��

# rule 18
	s/k01(i0[37])/k21$1/g;  # �Դ� -> �۴�
	s/k02(i0[37])/k21$1/g;  # ��� -> ����
	s/k03(i0[37])/k21$1/g;  # ����� -> ����
	s/k09(i0[37])/k21$1/g;  # �ܴ� -> ���

	s/k07(i0[37])/k04$1/g;  # �ݴ� -> �ܴ�
	s/k19(i0[37])/k04$1/g;  # ���� -> ����
	s/k20(i0[37])/k04$1/g;  # �ִ� -> �δ�
	s/k22(i0[37])/k04$1/g;  # ���ۿ� -> ���ۿ�
	s/k23(i0[37])/k04$1/g;  # �Ѵ� -> �˴�
	s/k25(i0[37])/k04$1/g;  # �ٴ� -> �д�
	s/k27(i0[37])/k04$1/g;  # ���� -> ����

	s/k17(i0[37])/k16$1/g;  # �买 -> �㹰
	s/k26(i0[37])/k16$1/g;  # �ո��� -> �ϸ���
	s/k11(i0[37])/k16$1/g;  # ���  -> ���
	s/k14(i0[37])/k16$1/g;  # ���� -> ����
	s/k18(i0[37])/k16$1/g;  # ���Ŵ� -> ���Ŵ�

# rule 19

	s/k16i06/k16i03/g; # ��� -> ���
	s/k21i06/k21i03/g; # ���� -> ����
	s/k01i06/k21i03/g; # �鸮 -> ���
	s/k17i06/k17i03/g; # ���� -> ����


#�ʼ�
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19
#�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��

#�߼�
# 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21
# �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��

#����
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23
#�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� 
#24 25 26 27
#�� �� �� ��

# rule 20�� ����
	s/i12j20k00i01j07k04i06j01k04/i12j20k00i01j07k04i03j01k04/g; # �ǰ߶�
	s/i12j21k16i13j21k04i06j01k04/i12j21k16i13j21k04i03j01k04/g; # ������
	s/i10j02k21i10j01k04i06j03k21/i10j02k21i10j01k04i03j03k21/g; # ���귮
	s/i01j07k08i04j01k04i06j07k01/i01j07k08i05j01k04i03j07k01/g; # ��ܷ�
	s/i01j09k21i01j15k04i06j07k01/i01j09k21i02j15k04i03j07k01/g; # ���Ƿ�
	s/i04j09k21i12j15k04i06j07k21/i04j09k21i12j15k04i03j07k21/g; # ������
	s/i10j01k21i01j07k04i06j08k00/i10j01k21i01j07k04i03j08k00/g; # ��߷�
	s/i19j12k21i04j01k04i06j09k00/i19j12k21i04j01k04i03j09k00/g; # Ⱦ�ܷ�
	s/i12j21k00i12j15k04i06j09k04/i12j21k00i12j15k04i03j09k04/g; # �̿���
	s/i12j21k17i12j15k04i06j13k00/i12j21k00i08j15k04i03j13k00/g; # �Կ���
	s/i01j14k00i01j19k04i06j18k00/i01j14k00i01j19k04i03j18k00/g; # ���ٷ�

# rule 20

	s/k04i06/k08i06/g; # ���� -> ����
	s/k08i03/k08i06/g; # Į�� -> Į��

	s/k15i03/k08i06/g; # ��� -> �޸�
	s/k13i03/k08i06/g; # �ӳ� -> �ҷ�

	$nya=$_;
	$_=$merong;
	return $nya;

}


sub rule_ki
{
#�ʼ�
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19
#�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��

#�߼�
# 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21
# �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��

#����
#1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23
#�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� 
#24 25 26 27
#�� �� �� ��
	my $merong=$_;
	$_=join(' ',@_);


	while(/(.*?k(07|25|13))(i12j21.*)/)
	{
		my $first=$1;
		my $second=$3;

		my $isin = $joeo { $second };

		if( defined($isin) )
		{
# rule 17 ��ħ ��,���� ���糪 ���̻��� ���� �� �� ���յǴ� ���
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
# rule 17 ��ħ ��,���� ���� �� �� �������� ���糪 ���̻簡 �ƴѰ��
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
#rule 29 ���������ڿ� ��̰� �ƴ� ���� ��,��,��,��,�� ���� �ö� �� ÷��.. 
				$second=~s/^i12/i03/g;
				$_=rule_ki_imbibition($first.$second);
			}
			else
			{
# rule 15 ��ħ �ڿ� ���� ��,��,��,��,�ϵ�� ���۵Ǵ� ���� ���¼Ұ� �ö� 
#         ��ħ�� ��ǥ������ �ٲپ� ����
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

# ������ ���ؼ� ��ħ�� �ٲ�� ��

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

# ������ ���� ��ħ�� �� �������� ������ ��ġ�°�
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

# rule 13 ����
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
#�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� 
#24 25 26 27
#�� �� �� ��

	$_=join(' ',@_);

# �Ʒ� ��Ģ�� ���� ��̰ų� �ڿ� ������ �ö��� ����
# ����� ���ܰ� �ִ�.
	s/k02/k01/g;
	s/k03/k01/g;
	s/k05/k04/g;
	s/k06/k04/g; # ���� ���� ���� ����
	s/k09/k01/g;
	s/k10/k16/g;
	s/k11/k08/g; # �� ��� ���� ����
	s/k12/k08/g;
	s/k13/k08/g;
	s/k14/k17/g;
	s/k15/k08/g; # ���� �� �� ���� ����
	s/k18/k17/g;
	s/k19/k07/g;
	s/k20/k07/g;
	s/k22/k07/g;
	s/k23/k07/g;
	s/k24/k01/g;
	s/k25/k07/g;
	s/k26/k17/g;
	s/k27/k07/g; #���� �ڿ����� ������ ���� ������ �����ȴ�.
	return $_;
}