for i in range(ord('A'), ord('Z')):
    print   i, chr(i)

    
create_ex_table(alpha):
    print "CREATE TABLE wExam_%c (" % (alpha)
    print """\
    id	int  not null,
                    doc_id	int  not null,
                    s_pos	int  not null,
                    s_len	smallint  not null,
                    pos1	smallint  not null,
                    len1	smallint  not null,
                    level tinyint  not null,
                    PRIMARY KEY(id, doc_id, s_pos)
