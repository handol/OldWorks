/*
	Symbol definition for dic. information.

	1. nouns --- 11 types(13 --- range: 0~12)

		N: Noun
		M: nuMeral
		X: auXiliary noun
		P: Pronoun

	2. verbs --- 77+2 types(79+2 --- range: 13~91+2)

		I: Intransitive verb
		T: Transitive verb
		V: aux. Verb
		J: adJective
		K: aux. adjective
		S: paSsive verb
		C: Causitive verb

		g: 'geora' irregular
		n: 'neora' irregular
		d: d-irregular
		b: b-irregular
		s: s-irregular
		h: h-irregular
		r: r-irregular
		l: l-irregular

	3. others --- 6 types(6 --- range: 92+2~97+2)

		A: Adverb
		E: dEterminer
		L: excLamation
*/

char *dicpos[NO_POS] = {
	"",
	"N",
	"NX",
	"X",
	"NP",
	"M",
	"P",
	"NM",
	"NMX",
	"NXP",
	"",
	"XP",
	"NMXP",
	"J",	/* verb begins here */
	"I",
	"T",
	"IT",
	"Jb",
	"S",
	"Jh",
	"C",
	"Tl",
	"",
	"IJ",
	"TJ",
	"",
	"Jl",
	"IS",
	"Ig",
	"CT",
	"ITJ",
	"Il",
	"In",
	"CS",
	"Td",
	"Tg",
	"Tn",
	"Ts",
	"",
	"CIS",
	"TV",
	"ST",
	"Tb",
	"Id",
	"CI",
	"CST",
	"CIST",
	"Jr",
	"Is",
	"IlTl",
	"TlJl",
	"IST",
	"Ib",
	"V",
	"K",
	"IsTs",
	"InTn",
	"KIV",
	"IV",
	"ITV",
	"CIT",
	"KTV",
	"KV",
	"IgTg",
	"IgV",
	"IbTb",
	"IdT",
	"CITVJ",
	"IlT",
	"IdTd",
	"ITs",
	"IIdTd",
	"KVJ",
	"IlTlJl",
	"ITTd",
	"IJ",
	"JbIJ",
	"TJ",
	"IsTTs",
	"JK",
	"TbJ",
	"InV",
	"IlIrTlJl",
	"IIg",
	"KIgVJ",
	"ITg",
	"CITV",
	"KVI",
	"TTd",
	"ITl",
	"Tu",
	"TS",
	"JsIs",	/* HANDIC added for 'DbUGb' */
	"TKVJ",	/* USERDIC added for 'QlUhbGb' */
	"A",	/* adv, det, excl begins here */
	"E",
	"L",
	"AL",
	"AE",
	"EL"
};
