#ifndef SIMPLE_STR_H
#define SIMPLE_STR_H
#include <stdio.h>
#include <string.h>

#define SIMPLE_STR_MAX	(511)
class simpleStr
{
	public:
		simpleStr() {
			buf[0] = 0;
			len = 0;
			ptr = buf;
		}
		void	add(char *str, int _len=0) ;
		void	add(int	ch) ;
		void print() {
			printf("%s\n", buf);
		}
		
		int	len;
		char buf[SIMPLE_STR_MAX+1];
		
	private:
		char *ptr;
};
