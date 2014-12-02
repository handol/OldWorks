
// HTML Tag, Entity
#ifndef HTML_TAG_H
#define HTML_TAG_H
typedef struct  _entityProc {
	char name[10];
	int	id; // decimal number for the entity
	char conv[10];
} stEntityProc;

typedef struct _tagProc {
	char name[12];
	int	pair;
	int	proc;
	int	newline;
} stTagProc;

#define	Opt_PAIR	2
#define	Must_PAIR	1
#define	Not_PAIR	0

#define	Proc_IGNORE		0
#define	Proc_NORMAL		1
#define	Proc_SPECIAL		2
#define	Proc_EMPHASIS		3
#define	Proc_ACRONOM		4
#define	Proc_TAB		5

class htmlTagEntity {
private:

	#define MAX_ENTS	(100)
	static	stEntityProc entities[MAX_ENTS]; // sort by name
	static	stEntityProc *entities_id[MAX_ENTS]; // sort by id (number)
	static	int	numEnts;

	#define MAX_TAGS	(100)
	static	stTagProc tags[MAX_TAGS];
	static	int	numTags;

	
public:
	static	stTagProc *BODY_tag, *A_tag, *B_tag, *FRAME_tag, *AREA_tag, *DT_tag;
	static	stTagProc *P_tag, *BR_tag, *HR_tag, *PRE_tag, *TITLE_tag, *H1_tag;
	
	static	void	init(char *homepath=0);
	static	int load_tag_list(char *fname);
	static	void print_tag(stTagProc *ptr);
	static	void prn_tag_list();
	static	int comp_tags(const void *a, const void *b);
	static	void sort_tags(stTagProc *arr, int size);
	static	stTagProc * tag_idx (const char *tag);
	static	void init_tag_list(char *fname);
	static	int load_entity_list(char *fname);
	static	void print_entity(stEntityProc *ptr);
	static	void prn_entity_list();
	static	void prn_entity_id_list();
	static	int comp_entities(const void *a, const void *b);
	static	void sort_entities(stEntityProc *arr, int size);
	static	int comp_entities_id(const void *a, const void *b);
	static	void sort_entities_id(stEntityProc **arr, int size);
	static	stEntityProc * entity_idx (const char *entity);
	static	stEntityProc * entity_id_idx (int id);
	static	void init_entity_list(char *fname);

	static	void find_link_tag();
};

#endif
