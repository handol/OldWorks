if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[test]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[test]
GO

CREATE TABLE [dbo].[test] (
	[name] [char] (10) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[age] [smallint] NULL ,
	[score] [int] NULL 
) ON [PRIMARY]
GO

