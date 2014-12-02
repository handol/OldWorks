
class	StrCmp 
{
	public:
		static void sort(char **arr, char size);
		static int anyncmp(char *A, char *B, int len);
		static int anycmp(char *A, char *B);
		static int wildcmp(char *A, char *B);
		static int URLanycmp(char *A, char *B);
		static int URLanyncmp(char *A, char *B, int len);
		static int URLwildcmp(char *A, char *B);
		static int match(char *A, char *B, int lenA, int lenB);
		
	private:
		static int _cmpstr (const void *a, const void *b);
};

