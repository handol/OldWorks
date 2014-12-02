
#ifndef ONE_DOC_H
#define ONE_DOC_H
typedef struct _one_doc {
	int	doc_head; //'A'..'Z' or '0'
	int	doc_id;
	int	doc_type;
	int	category;
	int	host_id;
	char	*url_path;
	char	*subject;
	char	*title;
	char	*file_path;
	int	html_size;
	int	text_size;
	int	hlink_chars;
	int	file_size;
	int	no_links;
	int	processed;
	int	doc_level;
	int	level_sum;
	int	no_examples;
	int	no_paragraphs;
	int	no_sentences;
	int	no_uniq_words; // example이 구해진 단어 갯수 
	int	is_glossary;
} oneDoc;

#endif
