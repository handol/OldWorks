#ifndef Spliter_H
#define Spliter_H
#include <string.h>
/* string  split iterator.
	splits a given string with a given separator, and produce each element per iteration.
*/

/*
ACE���� �����Ǵ� ��� ��� ����. --> string �ȿ� null ���� �־ ����� ó���Ǵ����� �ǹ�.
Example:

	 char buf[30];
	 ACE_OS::strcpy(buf, "William/Joseph/Hagins");

	 ACE_Tokenizer tok (buf);
	 tok.delimiter_replace ('/', 0);
	 for (char *p = tok.next (); p; p = tok.next ())
	   cout << p << endl;

This will print out:

William
Joseph
Hagins

*/
class Spliter
{
	public:
		Spliter(char *str, int len, int separator)
		{
			_is_end = 0;
			_i = 0;
			_count = 0;

			_ptr = str;
			_sep = separator;					  // default separator
			if (len==0) _orglen = strlen(str);
			else _orglen = len;
		}
		~Spliter()
		{
		}

		int	isend() {
			return (_is_end  || _i >= _orglen);
		}
		
		int getNext(char *outbuf, int maxlen)
		{
			return getNext(outbuf, maxlen, _sep);
		}

		int getNext(char *outbuf, int maxlen, int separator);
		int getNext(char *outbuf, int maxlen, char *separators);
		int skipSpace();
		int skipIMANspecial();
		int getLeftover(char *outbuf, int maxlen);
		static int  trim(char *dest, char *src);

	private:
		char    *_ptr;
		int _sep;
		int _orglen;
		int _count;								  // split�� ���� ������ element�� ����
		int _is_end;							  // string�� ������ �����ߴ°�
		int _i;									  // �־��� string����  �� �������� ó���� ����Ʈ ��


};
#endif
