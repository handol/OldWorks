/**

virtual functions. abstract class.

http://www.mines.edu/~crader/cs262/VirtualFunctions.html
*/

#ifndef Comparable_H
#define	Comparable_H
class  Comparable
{
public:	
	Comparable (void) {}
	//Comparable (Comparable &cp) {}
	virtual ~Comparable (void) {}
	
	virtual int	compare(Comparable *other)=0;
	virtual void print()=0;
};

#endif
