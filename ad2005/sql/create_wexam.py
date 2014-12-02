#for i in range(ord('A'), ord('Z')):
#	print   i, chr(i)

def delete_ex_table():
  for i in range(ord('A'), ord('Z')+1):
    print "DELETE FROM wExam_%c;" % (i)

def drop_ex_table():
  for i in range(ord('A'), ord('Z')+1):
    print "DROP TABLE wExam_%c;" % (i)
    
def create_ex_table(alpha):
	print "CREATE TABLE wExam_%c (" % (alpha)
	print """\
	id	int  not null,
	doc_id	int  not null,
	s_pos	int  not null,
	s_len	smallint  not null,
	pos1	smallint  not null,
	len1	smallint  not null,
	level tinyint  not null,
	conj tinyint  not null,
	PRIMARY KEY(id, doc_id, s_pos) );
	GO"""
	print


delete_ex_table()
print
print
for i in range(ord('A'), ord('Z')+1):
	create_ex_table ( chr(i) )
