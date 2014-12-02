IF EXISTS (SELECT name FROM master.dbo.sysdatabases WHERE name = N'anydict')
	DROP DATABASE [anydict]
GO

CREATE DATABASE [anydict]  ON (NAME = N'anydict_Data', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL\data\anydict_Data.MDF' , SIZE = 1, FILEGROWTH = 10%) LOG ON (NAME = N'anydict_Log', FILENAME = N'C:\Program Files\Microsoft SQL Server\MSSQL\data\anydict_Log.LDF' , SIZE = 1, FILEGROWTH = 10%)
 COLLATE Korean_Wansung_CI_AS
GO

exec sp_dboption N'anydict', N'autoclose', N'false'
GO

exec sp_dboption N'anydict', N'bulkcopy', N'false'
GO

exec sp_dboption N'anydict', N'trunc. log', N'false'
GO

exec sp_dboption N'anydict', N'torn page detection', N'true'
GO

exec sp_dboption N'anydict', N'read only', N'false'
GO

exec sp_dboption N'anydict', N'dbo use', N'false'
GO

exec sp_dboption N'anydict', N'single', N'false'
GO

exec sp_dboption N'anydict', N'autoshrink', N'false'
GO

exec sp_dboption N'anydict', N'ANSI null default', N'false'
GO

exec sp_dboption N'anydict', N'recursive triggers', N'false'
GO

exec sp_dboption N'anydict', N'ANSI nulls', N'false'
GO

exec sp_dboption N'anydict', N'concat null yields null', N'false'
GO

exec sp_dboption N'anydict', N'cursor close on commit', N'false'
GO

exec sp_dboption N'anydict', N'default to local cursor', N'false'
GO

exec sp_dboption N'anydict', N'quoted identifier', N'false'
GO

exec sp_dboption N'anydict', N'ANSI warnings', N'false'
GO

exec sp_dboption N'anydict', N'auto create statistics', N'true'
GO

exec sp_dboption N'anydict', N'auto update statistics', N'true'
GO

if( ( (@@microsoftversion / power(2, 24) = 8) and (@@microsoftversion & 0xffff >= 724) ) or ( (@@microsoftversion / power(2, 24) = 7) and (@@microsoftversion & 0xffff >= 1082) ) )
	exec sp_dboption N'anydict', N'db chaining', N'false'
GO

use [anydict]
GO

if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[test]') and OBJECTPROPERTY(id, N'IsUserTable') = 1)
drop table [dbo].[test]
GO

if not exists (select * from master.dbo.syslogins where loginname = N'inisoft')
BEGIN
	declare @logindb nvarchar(132), @loginlang nvarchar(132) select @logindb = N'anydict', @loginlang = N'ÇÑ±¹¾î'
	if @logindb is null or not exists (select * from master.dbo.sysdatabases where name = @logindb)
		select @logindb = N'master'
	if @loginlang is null or (not exists (select * from master.dbo.syslanguages where name = @loginlang) and @loginlang <> N'us_english')
		select @loginlang = @@language
	exec sp_addlogin N'inisoft', null, @logindb, @loginlang
END
GO

if not exists (select * from dbo.sysusers where name = N'inisoft' and uid < 16382)
	EXEC sp_grantdbaccess N'inisoft', N'inisoft'
GO

exec sp_addrolemember N'db_owner', N'inisoft'
GO

CREATE TABLE [dbo].[test] (
	[name] [char] (10) COLLATE Korean_Wansung_CI_AS NOT NULL ,
	[age] [smallint] NULL ,
	[score] [int] NULL 
) ON [PRIMARY]
GO

