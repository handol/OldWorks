/*
for Anydict.com  dahee@netsgo.com
2002.4.2 ~
*/
// HTML Tag, Entity
#include <stdio.h>
#include <ctype.h>
#include <string.h> // strcasecmp()
#include <stdlib.h> // qsort()
#include "htmlTagEntity.h"
#include "StrUtil.h"
#include "MemSplit.h"


#ifdef WIN32
#include "windowsDef.h"
#endif


#define PRN	printf

stEntityProc htmlTagEntity::entities[MAX_ENTS]; // sort by name
stEntityProc *htmlTagEntity::entities_id[MAX_ENTS]; // sort by id (number)
int	htmlTagEntity::numEnts = 0;

stTagProc htmlTagEntity::tags[MAX_TAGS];
int	htmlTagEntity::numTags = 0;

stTagProc *htmlTagEntity::BODY_tag=0, *htmlTagEntity::A_tag=0, *htmlTagEntity::B_tag=0, *htmlTagEntity::FRAME_tag=0, *htmlTagEntity::AREA_tag=0, *htmlTagEntity::DT_tag=0;
stTagProc *htmlTagEntity::P_tag=0, *htmlTagEntity::BR_tag=0, *htmlTagEntity::HR_tag=0, *htmlTagEntity::PRE_tag=0, *htmlTagEntity::TITLE_tag=0, *htmlTagEntity::H1_tag=0;

void htmlTagEntity::init(char *homepath)
{
	char  file[256];

	StrUtil::path_merge(file, homepath, "tag.dat");
	htmlTagEntity::init_tag_list(file);
	
	StrUtil::path_merge(file, homepath, "entity.dat");
	htmlTagEntity::init_entity_list(file);

	htmlTagEntity::find_link_tag();
}

int htmlTagEntity::load_tag_list(char *fname)
{
	FILE *fp;
	char buf[1024];
	char tmp[32];
	char *ptr;
	int	n=0;
	int	lines=0;
	
	memset(tags, 0, sizeof(tags));
	if ( (fp = fopen(fname, "r")) == NULL)  {		
		PRN("cannot read %s\n", fname);		
		return -1;
	}
	
	while(fgets(buf, 1024, fp) != NULL) {
		lines++;
		if (StrUtil::is_cmnt_line(buf) ) continue;
		sscanf(buf, "%s", tmp);
		
		strncpy(tags[n].name, tmp, 10);
		if ( (ptr=strchr(buf, ',')) == 0) {
			PRN("!Error: line %d\n", lines );
			continue;
		}
		sscanf(ptr+1, "%s", tmp);
		if (strncasecmp(tmp, "NP", 2)==0)
			tags[n].pair = Not_PAIR;	
		else if (strncasecmp(tmp, "OP", 2)==0)
			tags[n].pair = Opt_PAIR;	
		else
			tags[n].pair = Must_PAIR;

		if ( (ptr=strchr(ptr+3, ',')) == 0) {
			//PRN("!Error: line %d\n");
			continue;
		}

		tags[n].proc = strtol(ptr+1, NULL, 10);
		if (strstr(ptr+3, "NEW"))
			tags[n].newline = 1;
		n++;
		// PRN("%s: proc %d, New %d\n", tags[n].name, tags[n].proc, tags[n].newline);
	}
	numTags = n;
	fclose(fp);
	return 0;
}

void htmlTagEntity::print_tag(stTagProc *ptr)
{
	PRN("%s: pair %d, proc %d, New %d\n",
		ptr->name, ptr->pair, ptr->proc, ptr->newline);
}

void htmlTagEntity::prn_tag_list()
{
	int	i;
	PRN("--- tag list ---\n");
	for(i=0; i<numTags; i++)
		print_tag(tags+i);
		PRN("%s: pair %d, proc %d, New %d\n",
			tags[i].name, tags[i].pair, tags[i].proc, tags[i].newline);
}

// sort tag process data
int htmlTagEntity::comp_tags(const void *a, const void *b)
{
	return strcasecmp( ((stTagProc *)a)->name, ((stTagProc *)b)->name);
}

void htmlTagEntity::sort_tags(stTagProc *arr, int size)
{
	qsort(arr, size, sizeof(stTagProc), comp_tags);
}

// search tags
/* Return index of etities if it is a valid etities, or -1 otherwise.  
   etities is looked up in `commands' using binary search algorithm.  */
stTagProc * htmlTagEntity::tag_idx (const char *tag)
{
  int min = 0, max = numTags;

  do
    {
      int i = (min + max) / 2;
      int cmp = strcasecmp (tag, tags[i].name);
      if (cmp == 0)
	return (tags+i);
      else if (cmp < 0)
	max = i - 1;
      else
	min = i + 1;
    }
  while (min <= max);
  return NULL;
}

void htmlTagEntity::init_tag_list(char *fname)
{
	if (numTags==0) {
		load_tag_list(fname);
		sort_tags(tags, numTags);
	}
#ifdef BUG
	 //prn_tag_list();
#endif		
}

//--------------------------------------------------------------------


int htmlTagEntity::load_entity_list(char *fname)
{
	FILE *fp;
	char buf[1024];
	int	entity_cnt=0, lineno=0;
	MemSplit	line(4, 32);
	
	memset(entities, 0, sizeof(entities));
	if ( (fp = fopen(fname, "r")) == NULL)  {		
		PRN("cannot read %s\n", fname);		
		return -1;
	}

		
	while(fgets(buf, 1024, fp) != NULL) {
		lineno ++;

		if (StrUtil::is_cmnt_line(buf) ) continue;

		line.split(buf);
		if (line.num() < 4) {
			PRN("line [%d]: entity erorr\n", lineno);
			continue;
		}

		StrUtil::copy_alphabet(entities[entity_cnt].name, line.str(0) + 1, 10);
		entities[entity_cnt].id = strtol(line.str(1)+2, NULL, 10);
		if (line.str(3)[0]==0)
			// coversion string is null --> space (&nbsp;)
			strcpy(entities[entity_cnt].conv, " ");
		else
			strcpy(entities[entity_cnt].conv, line.str(3));
		
		
		entity_cnt++;
	}
	fclose(fp);
	numEnts = entity_cnt;
	return entity_cnt;
}

void htmlTagEntity::print_entity(stEntityProc *ptr)
{
	PRN("&%s; &#%d; --> %s\n",
			ptr->name, ptr->id, ptr->conv);
}

void htmlTagEntity::prn_entity_list()
{
	int	i;
	PRN("--- entity list ---\n");
	for(i=0; i<numEnts; i++)
		print_entity(entities+i);
}

void htmlTagEntity::prn_entity_id_list()
{
	int	i;
	PRN("--- entity ID list ---\n");
	for(i=0; i<numEnts; i++) {
		print_entity( entities_id[i] );
	}
}

// sort entity process data

int htmlTagEntity::comp_entities(const void *a, const void *b)
{
	return strcasecmp( ((stEntityProc *)a)->name, ((stEntityProc *)b)->name);
}

void htmlTagEntity::sort_entities(stEntityProc *arr, int size)
{
	qsort(arr, size, sizeof(stEntityProc), comp_entities);
}

// sort entity process data
int htmlTagEntity::comp_entities_id(const void *a, const void *b)
{
	return ( (*(stEntityProc **)a)->id -  (*(stEntityProc **)b)->id);
}

void htmlTagEntity::sort_entities_id(stEntityProc **arr, int size)
{
	qsort(arr, size, sizeof(stEntityProc *), comp_entities_id);
}

/* Return index of etities if it is a valid etities, or -1 otherwise.  
   etities is looked up in `commands' using binary search algorithm.  */
stEntityProc * htmlTagEntity::entity_idx (const char *entity)
{
  int min = 0, max = numEnts;

	//PRN("Entity : %s %d\n", entity, strlen(entity));
  do
    {
      int i = (min + max) / 2;
      int cmp = strcasecmp (entity, entities[i].name);
      //PRN("strcmp %d: %s %s --> %d\n",  i,entity, entities[i].name, cmp);
      if (cmp == 0)
	return (entities+ i);
      else if (cmp < 0)
	max = i - 1;
      else
	min = i + 1;
    }
  while (min <= max);
  return NULL;
}


/* Return index of etities if it is a valid etities, or -1 otherwise.  
   etities is looked up in `commands' using binary search algorithm.  */
stEntityProc * htmlTagEntity::entity_id_idx (int id)
{
  int min = 0, max = numEnts;

  do
    {
      int i = (min + max) / 2;
      int cmp = id - entities[i].id;
      if (cmp == 0)
	return (entities + i);
      else if (cmp < 0)
	max = i - 1;
      else
	min = i + 1;
    }
  while (min <= max);
  return NULL;
}

void htmlTagEntity::init_entity_list(char *fname)
{
	int	i;
	if (numEnts==0) {
		load_entity_list(fname);
		//prn_entity_list();
		for(i=0; i<numEnts; i++)
			entities_id[i] = entities+i;
		sort_entities(entities, numEnts);
		sort_entities_id(entities_id, numEnts);
		// prn_entity_list();
		// prn_entity_id_list();
	}
}

void htmlTagEntity::find_link_tag()
{
	// tags that may have a hyper link
	if (A_tag) return;

	BODY_tag = tag_idx("BODY");
	A_tag = tag_idx("A");
	B_tag = tag_idx("B");
	DT_tag = tag_idx("DT");
	BR_tag = tag_idx("BR");
	FRAME_tag = tag_idx("FRAME");
	AREA_tag = tag_idx("AREA");
	
	P_tag = tag_idx("P");
	H1_tag = tag_idx("H1");
	HR_tag = tag_idx("HR");
	PRE_tag = tag_idx("PRE");
	TITLE_tag = tag_idx("TITLE");
}

