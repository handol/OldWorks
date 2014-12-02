import pyodbc

#cnxn = pyodbc.connect('DRIVER={SQL Server};SERVER=localhost;DATABASE=anydict;UID=inisoft;PWD=gksehf')

cnxn = pyodbc.connect("DSN=ANYDICT;UID=inisoft;PWD=gksehf")  ## ODBC 이용 연결


#print cnxn
print cnxn.getinfo(pyodbc.SQL_DBMS_NAME)


cursor = cnxn.cursor()
cursor.execute("select count(*) from docs")
#cursor.execute("desc table  docs")
#print "--> %d rows"
for row in cursor:
    print row[0]
