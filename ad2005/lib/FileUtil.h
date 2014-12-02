#ifndef	FILEUTIL_H
#define	FILEUTIL_H
class FileUtil
{
public:
	static	int get_file_size(char *fname);
	static	int check_file_exist(char *fname);
	static	int check_this_directory(char *fname);

	static	int load_file_2_new_mem(char *fname, char **mem_ptr);
	static	int load_file_2_given_mem(char *mem, char *fname, int max_size);
	static	int load_file_2_given_mem_skip_first_line(char *mem, char *fname, int max_size);

	static	int save_file_2_from_mem(char *mem, char *fname);
	static	int append_file_2_from_mem(char *mem, char *fname);

	static	int	file_read_lines(char *fname);

	static	int	countLines(char *fname);
};
#endif
