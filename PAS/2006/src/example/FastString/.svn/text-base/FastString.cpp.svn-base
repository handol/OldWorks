#include <FastString.hpp>
#include <stdio.h>
#include <stdlib.h>

template <size_t SIZE>
int doTest();

int main()
{
	doTest<1>();
	doTest<2>();
	doTest<3>();
	doTest<4>();
	doTest<15>();
	doTest<16>();
	doTest<17>();
	doTest<19>();
	doTest<20>();
	doTest<21>();
	doTest<25>();
	doTest<26>();
	doTest<27>();
	doTest<35>();
	doTest<36>();
	doTest<37>();
	doTest<64>();

	return 0;
}

template <size_t SIZE>
int doTest()
{
	FastString<SIZE> str;

	const char testString[] = "abcdefghijklmnopqrstuvwxyz0123456789";

	const int tmpStringLen = 256;
	char tmp1String[tmpStringLen];
	char tmp2String[tmpStringLen];
	char tmp3String[tmpStringLen];

	assert(strlen(testString) < tmpStringLen);
	assert(str.maxSize() < tmpStringLen);

	// assign + append
	strcpy(tmp1String, testString);
	for(int i = str.maxSize()+2; 0 <= i; --i)
	{
		// assign
		assert(i < sizeof(tmp1String));
		tmp1String[i] = '\0';
		str = tmp1String;

		if(strlen(tmp1String) <= str.maxSize())
			assert(strcmp(str, tmp1String) == 0);
		else
			assert(strcmp(str, tmp1String) != 0);

		// append
		strcpy(tmp2String, testString);
		for(int j = str.maxSize()+2; 0 <= j; --j)
		{
			str = tmp1String;

			assert(j < sizeof(tmp2String));
			tmp2String[j] = '\0';
			str += tmp2String;

			memset(tmp3String, 0, sizeof(tmp3String));
			strncpy(tmp3String, tmp1String, SIZE);
			strncat(tmp3String, tmp2String, SIZE-strlen(tmp3String));

			assert(strcmp(str, tmp3String) == 0);
		}
	}

	// insert
	str = "abcdefg";

	for(int i = str.maxSize()+2; 0 <= i; --i)
	{
		str = "abcdefg";
		str.insert(i, "12345");
	}

	for(int i = str.maxSize()+2; 0 <= i; --i)
	{
		str = "abcdefghijklmnopqrstuvwxyz";
		str.insert(i, "12345");
	}

	// erase
	for(int i = 0; i < str.maxSize()+2; ++i)
	{
		for(int j = 0; j < str.maxSize()+2; ++j)
		{
			str = "abcdefghijklmnopqrstuvwxyz";
			str.erase(i, j);
		}
	}

	// clear
	str = testString;
	str.clear();
	assert(str.size() == 0);

	// sprintf
	str.sprintf("%s : %d", "Age", 28);
	snprintf(tmp1String, str.maxSize()+1, "%s : %d", "Age", 28);
	assert(strcmp(str, tmp1String) == 0);

	str.sprintf("HP : %d-%d-%d", 111, 2222, 3333);
	snprintf(tmp1String, str.maxSize()+1, "HP : %d-%d-%d", 111, 2222, 3333);
	assert(strcmp(str, tmp1String) == 0);

	str.sprintf("%s : %d, %s", "Age", 28, "abcdefghijklmnopqrstuvwxyz01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	snprintf(tmp1String, str.maxSize()+1, "%s : %d, %s", "Age", 28, "abcdefghijklmnopqrstuvwxyz01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	assert(strcmp(str, tmp1String) == 0);

	// assign number
	str = 2006;
	snprintf(tmp1String, str.maxSize()+1, "%d", 2006);
	assert(strcmp(str, tmp1String) == 0);

	str = 365.25;
	snprintf(tmp1String, str.maxSize()+1, "%f", 365.25);
	assert(strcmp(str, tmp1String) == 0);

	// append number
	str = 2007;
	str += 1231;
	snprintf(tmp1String, str.maxSize()+1, "%d%d", 2007, 1231);
	assert(strcmp(str, tmp1String) == 0);

	str = 365.25;
	str += 100.0;
	snprintf(tmp1String, str.maxSize()+1, "%f%f", 365.25, 100.0);
	assert(strcmp(str, tmp1String) == 0);

	// assign faststring
	str = "01234";
	str += FastString<10>("abcdefghijklmn");
	snprintf(tmp1String, str.maxSize()+1, "%s%s", "01234", "abcdefghij");
	assert(strcmp(str, tmp1String) == 0);

	// trim
	str = "365.25\t\r\n   \t\t  \t\r\n";
	str.trim();
	snprintf(tmp1String, str.maxSize()+1, "365.25");
	assert(strcmp(str, tmp1String) == 0);

	const char whiteSpaceTestString[] = "\t\r\n \t\r\n365.25\t\r\n \t\r\n";
	if(strlen(whiteSpaceTestString) <= str.maxSize())
	{
		str = whiteSpaceTestString;
		str.trim();
		snprintf(tmp1String, str.maxSize()+1, "365.25");
		assert(strcmp(str, tmp1String) == 0);
	}

	// casting
	if(4 <= str.maxSize())
	{
		str = 2007;
		assert(str.toInt() == 2007);
	}

	if(6 <= str.maxSize())
	{
		str = 365.25;
		assert(str.toFloat() == 365.25);
	}
		
	// substr
	for(size_t i = 0; i < str.maxSize()+2; ++i)
	{
		for(size_t j = 0; j < str.maxSize()+2; ++j)
		{
			FastString<16> subStr = str.substr(i, j);
			assert(subStr.maxSize() < sizeof(tmp1String));

			if(i < str.size())
			{
				memset(tmp1String, 0, sizeof(tmp1String));
				strncpy(tmp1String, &str[i], std::min(subStr.maxSize(), j));
			}
			else
			{
				tmp1String[0] = '\0';
			}

			assert(strcmp(subStr, tmp1String) == 0);
		}
	}

	// find
	str = "01234567890123456789";
	for(size_t i = 0; i < str.size()+2; ++i)
	{
		str.find('0', i);
		str.find("123", i);
	}

	if(20 <= str.maxSize())
	{
		assert(str.find('0') == 0);
		assert(str.find('1') == 1);
		assert(str.find('2') == 2);
		assert(str.find('9') == 9);
		assert(str.find('a') == -1);
		assert(str.find("012") == 0);
		assert(str.find("890") == 8);
		assert(str.find("abc") == -1);
		assert(str.find("0", 10) == 10);
		assert(str.find("1", 10) == 11);
		assert(str.find("2", 10) == 12);
		assert(str.find("9", 10) == 19);
		assert(str.find("a", 10) == -1);
		assert(str.find("89", 10) == 18);
		assert(str.find("12", 10) == 11);
		assert(str.find("789", 10) == 17);
		assert(str.find("abc", 10) == -1);
	}

	// split
	str = "012345678901234567890";
	for(size_t i = 0; i < str.size()+2; ++i)
	{
		FastString<16> subStr;
		str.split(&subStr, 'a', i);
		str.split(&subStr, "ab", i);
	}

	if(20 <= str.maxSize())
	{
		FastString<16> subStr;

		str.split(subStr, '0');
		assert(subStr == "");

		str.split(subStr, '1');
		assert(subStr == "0");

		str.split(subStr, '3');
		assert(subStr == "012");

		str.split(subStr, '3', 9);
		assert(subStr == "9012");

		str.split(subStr, "abc");
		assert(subStr == "0123456789012345");

		str.split(subStr, "0");
		assert(subStr == "");

		str.split(subStr, "567");
		assert(subStr == "01234");

		str.split(subStr, "568");
		assert(subStr == "0123456789012345");
	}

	// getLine

	str = "012345678901234567890";
	for(size_t i = 0; i < str.size()+2; ++i)
	{
		FastString<16> subStr;
		str.getLine(&subStr, i);
	}

	if(20 <= str.maxSize())
	{
		str = "0123\nabcd\nefg";
		FastString<16> subStr;
		int pos;

		pos = str.getLine(&subStr);
		assert(subStr == "0123");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "abcd");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "efg");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "");

		str = "0123\r\nabcd\r\nefg";

		pos = str.getLine(&subStr);
		assert(subStr == "0123");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "abcd");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "efg");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "");

		str = "0123\n\nabcd\nefg\n\n";

		pos = str.getLine(&subStr);
		assert(subStr == "0123");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "abcd");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "efg");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "");

		pos = str.getLine(&subStr, pos+1);
		assert(subStr == "");
		assert(pos == -1);

		str = "abcde";
		str.getLine(&subStr);
		assert(str == subStr);
	}

	// replace
	if(20 <= str.maxSize())
	{
		FastString<20> subStr;

		str = "01010101";
		subStr = str.replace("0", "1");
		assert(subStr == "11111111");

		str = "01201230123";
		subStr = str.replace("1", "ab");
		assert(subStr == "0ab20ab230ab23");

		str = "01234567890";
		subStr = str.replace("456", "abcdef");
		assert(subStr == "0123abcdef7890");

		str = "01234567890";
		subStr = str.replace("0", "abc");
		assert(subStr == "abc123456789abc");
	}

	// random test
	str = FastString<4>("ab");
	str = FastString<32>("abcdefghijklmnopqrstuvwxyz");

	return 0;
}

