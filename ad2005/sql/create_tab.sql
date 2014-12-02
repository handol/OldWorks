
drop table hosts;
drop table docs;


CREATE TABLE test (
  name  char (18) not null ,
  age smallint  not null,
  score int not null,
 primary key(name)
);

CREATE TABLE hosts (
		hostname char(40) not null,
		sitename char(40),
		PRIMARY KEY(hostname)
);


CREATE TABLE docs (
      id int  not null ,
      url char(255) not null,
      title char(128) not null,      
      fpath char(255) not null,
      fetchtime datetime not null,
      fsize int  not null,
      processed tinyint not null,   
	level	tinyint  not null,
	ctg int not null,
	PRIMARY KEY(id)
);

CREATE INDEX docs_url_index ON docs(url);
GO

DROP TABLE cronjob;

CREATE TABLE cronjob (
  jobid int not null,
  dayofweek char(3)  not null,
  day char(3)  not null,
  hour char(3)  not null,
  minute char(3)  not null,
  second char(3)  not null,
  jobtype int not null,
  jobdesc char  (255) not null,
  PRIMARY KEY(jobid)
);
GO

DROP TABLE worddict;

CREATE TABLE worddict (
		word char(40) not null,
		word_id int  not null ,
		word_type	tinyint not null,
		level tinyint  not null,
		PS int  not null,
		is_conj tinyint  not null,
		org_word char(40),
		PRIMARY KEY(word_id)
	);

CREATE INDEX word_index ON worddict(word);
GO

DROP TABLE idiomdict;
CREATE TABLE idiomdict (
		idiom char(80) not null,
		idiom_id int   not null,
		PRIMARY KEY(idiom_id)
);

CREATE INDEX idiom_index ON idiomdict(idiom);
GO

DROP TABLE harvestreq;
CREATE TABLE harvestreq (
  id  int not null,
  req char(255) not null,
  result  varchar (512) not null,
  count int not null,
  tm  datetime not null,
  PRIMARY KEY(id)
);
CREATE INDEX harvestreq_tm ON harvestreq(tm);
GO

DROP TABLE extractreq;
CREATE TABLE extractreq (
  id  int not null,
  req char(255) not null,
  result  varchar (512) not null,
  count int not null,
  tm datetime not null,
  PRIMARY KEY(id)
);
CREATE INDEX extractreq_tm ON extractreq(tm);
GO

CREATE TABLE category (
		ctg	int  not null,
		name char(64) not null,
		descrip char(64) not null,
		 PRIMARY KEY(ctg)
);
GO


CREATE TABLE urltoctg (
		urlprefix char (64) not null,
		ctg	int  not null,
    PRIMARY KEY(urlprefix)
);
GO


DROP TABLE newhanword;
CREATE TABLE newhanword (
	word char(40) not null,
	word_id int  not null ,
	PRIMARY KEY(word_id)
);

CREATE INDEX newhanword_index ON newhanword(word);
